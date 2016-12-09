// author: Charles Smith <cas275@pitt.edu>
// a name server for tiny-google

#include "server.h"
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define ECHO      0
#define REQUEST   1
#define REGISTER  2
#define HEARTBEAT 3
#define SUPER_NODE_REQUEST   4
#define SUPER_NODE_REGISTER  5
#define SUPER_NODE_HEARTBEAT 6

#define MAX_SUPER_NODE 16
#define MAX_WORKERS    16

#define HEARTBEAT_WAIT_TIME 15

void *process_heartbeat(void *args);
void *process_super_node_heartbeat(void *args);

// worker list, and mutex for using the list
char workers[MAX_WORKERS][100];
int worker_heartbeat[MAX_WORKERS];
pthread_mutex_t worker_lock; 

// super_node list, and mutex for using the list
char super_nodes[MAX_SUPER_NODE][100];
int super_node_heartbeat[MAX_SUPER_NODE];
pthread_mutex_t super_node_lock;

// NOTE: since we're using an append on the name-server, 
// there may be entries that no longer exist.  Currently,
// we do not have any methods to clean these up.
void init_server(char * ip, int port){
    // set the workers to 0 
    int i;
    int j;
    for(i=0; i<MAX_WORKERS; i++){
        for(j=0; j<100; j++){
            workers[i][j] = '\0';
        }
    }

    for(i=0; i<MAX_SUPER_NODE; i++){
        for(j=0; j<100; j++){
            super_nodes[i][j] = '\0';
        }
    }

    printf("%s:%d\n",ip,port);
    FILE * fp = fopen("./ns.txt", "a");
    fprintf(fp, "%s:%d\n", ip, port);
    fclose(fp);

    // init the mutex
    if( pthread_mutex_init(&worker_lock, NULL) != 0){
        printf("error creating the mutex\n");
    }

    if(pthread_mutex_init(&super_node_lock, NULL) != 0){
        printf("error creating the super_node mutex\n");
    }

    // init the heartbeat
    pthread_t heartbeat;
    pthread_create(&heartbeat, NULL, process_heartbeat, NULL);

    pthread_t super_node_heartbeat;
    pthread_create(&super_node_heartbeat, NULL, process_super_node_heartbeat, NULL);
}

void *process_connection(void *args){
    int client_fd = *((int *) args);
    printf("\n");

    // find request type    
    int32_t request_type = 0;
    int res = read(client_fd, &request_type, sizeof(int32_t));
    if(res <= 0){
        printf("error reading, returned: %d\n", res);
        close(client_fd);
        free(args);
        pthread_exit(NULL);
    }

    request_type = ntohl(request_type);


    if(request_type == ECHO){
        char str[256];
        bzero(str, 256);
        read(client_fd, str, 255); // make sure we keep the '\0' at the end
        printf("read: %s\n", str);
        write(client_fd, str, 256);

    }


    else if(request_type == REQUEST){
        printf("request\n");

        // let's see how many workers they want
        int32_t num_workers_req = 0; // the number of requested workers
        read(client_fd, &num_workers_req, sizeof(int32_t));
        num_workers_req = ntohl(num_workers_req);
        printf("client is requesting %d workers\n", num_workers_req);
        if(num_workers_req <= 0){
            printf("number of workers requested is invalid\n");
            printf("closing\n");
            close(client_fd);
            free(args);
            pthread_exit(NULL);
        }

        // now it's time to see what we have
        int32_t available_workers = 0;
        int i = 0;
        pthread_mutex_lock(&worker_lock); // MUTEX LOCK HERE
        for(i=0; i<MAX_WORKERS; i++){
            if(workers[i][0]!='\0'){
                available_workers++;
                printf("%d %s\n", i, workers[i]);
            }
        }
        printf("available_workers = %d\n", available_workers);

        int32_t num_workers_to_send = 0;
        if(num_workers_req > available_workers){
            num_workers_to_send = available_workers;
        }
        else{
            num_workers_to_send = num_workers_req;
        }
        printf("sending %d workers\n", num_workers_to_send);
        num_workers_to_send = htonl(num_workers_to_send);
        write(client_fd, &num_workers_to_send, sizeof(int32_t));

        // now to send the worker information to the client
        num_workers_to_send = ntohl(num_workers_to_send);
        char str_to_send[101];
        for(i=0; i<MAX_WORKERS; i++){
            if(num_workers_to_send == 0){
                printf("sent all workers\n");
                break;
            }
            if(workers[i][0] != '\0'){
                sprintf(str_to_send, "%s;",workers[i]);
                printf("%s\n",str_to_send);
                write(client_fd,str_to_send,sizeof(char) * strlen(str_to_send));
                workers[i][0] = '\0';
                num_workers_to_send--;
                printf("num workes left %d\n", num_workers_to_send);
            }
        }

        pthread_mutex_unlock(&worker_lock);  // MUTEX UNLOCK HERE
    }


    else if(request_type == REGISTER || request_type == HEARTBEAT){
        printf("register or heartbeat\n");

        int32_t length;
        int res = read(client_fd, &length, sizeof(int32_t));
        if(res<=0){
            printf("could not read register length, returned: %d\n",res);
            printf("closing\n");
            close(client_fd);
            free(args);
            pthread_exit(NULL);
        }

        length = ntohl(length);
        if(length <= 0 || length >100){
            printf("recieved invalid length: %d\n", length);
            printf("closing\n");
            close(client_fd);
            free(args);
            pthread_exit(NULL);
        }

        // find a free spot to save the worker
        int i;
        int free_spot = MAX_WORKERS;

        pthread_mutex_lock(&worker_lock); // MUTEX HERE
        char new_worker[150];
        new_worker[149] = '\0';
        res = read(client_fd, new_worker, length * sizeof(char));
        for(i=0; i<MAX_WORKERS; i++){
            if(workers[i][0] == '\0'){
                free_spot = i;
            }
            else if(strcmp(workers[i],new_worker) == 0){ // worker is found
                printf("worker %s found at %i\n", new_worker, i);
                worker_heartbeat[i] = 1;
                pthread_mutex_unlock(&worker_lock);
                printf("closing\n");
                close(client_fd);
                free(args);
                pthread_exit(NULL);
            }
        }
        if(free_spot == MAX_WORKERS){ //max already reached
            pthread_mutex_unlock(&worker_lock); // UNLOCK 1
            printf("max workers already reached, current max = %d\n", MAX_WORKERS);
            printf("closing\n");
            close(client_fd);
            free(args);
            pthread_exit(NULL);
        }
        else{
            printf("saving worker to spot %d\n",free_spot);
        }

        // read and save the worker      
        bzero(workers[free_spot], 100);
        strncpy(workers[free_spot], new_worker, 100);
        printf("worker stored as %s\n", workers[free_spot]);
        worker_heartbeat[free_spot] = 1;
        pthread_mutex_unlock(&worker_lock);

    }
    else if(request_type == SUPER_NODE_REQUEST){
        printf("super node request\n");

        // we only give clients 1 supernode, so no need to ask how many they want
        int i;
        int sent = 0;
        pthread_mutex_lock(&super_node_lock);
        for(i=0; i<MAX_SUPER_NODE; i++){
            if(super_nodes[i][0] != '\0'){
                printf("sending super node %s at %d\n", super_nodes[i], i);
                write(client_fd, super_nodes[i], sizeof(char)*(strlen(super_nodes[i]) + 1));
                // if the super_node isn't multi-threaded, we remove it here
                sent = 1;
                break;
            }
        }
        pthread_mutex_unlock(&super_node_lock);
        if(sent == 0){
            printf("no node to send\n");
        }

    }
    else if(request_type == SUPER_NODE_REGISTER || request_type == SUPER_NODE_HEARTBEAT){
        printf("super_node register or super_node heartbeat\n");
        
        int i;

        int32_t length;
        int res = read(client_fd, &length, sizeof(int32_t));
        if(res <= 0){
            printf("error reading request\n");
            close(client_fd);
            free(args);
            pthread_exit(NULL);
        }
        
        length = ntohl(length);
        if(length <=0 || length >= 100){
            printf("invalid length\n");
            close(client_fd);
            free(args);
            pthread_exit(NULL);
        }

        char super_node_ip[101];
        res = read(client_fd, super_node_ip, sizeof(char) * length);
        if(res <= 0){
            printf("invalid read\n");
            close(client_fd);
            free(args);
            pthread_exit(NULL);
        }
        super_node_ip[100] = '\0';
        int free_spot = MAX_SUPER_NODE;
        pthread_mutex_lock(&super_node_lock);
        for(i=0; i<MAX_SUPER_NODE; i++){
            if(super_nodes[i][0] == '\0'){
                free_spot = i;
            }
            else if(strcmp(super_nodes[i],super_node_ip) == 0){
                printf("found %s at %d\n", super_node_ip, i);
                super_node_heartbeat[i] = 1;
                pthread_mutex_unlock(&super_node_lock);
                close(client_fd);
                free(args);
                pthread_exit(NULL);
            }       
        }
        if(free_spot == MAX_SUPER_NODE){
            printf("no space to store super node\n");
            pthread_mutex_unlock(&super_node_lock);
            close(client_fd);
            free(args);
            pthread_exit(NULL);
        }
        printf("saving super node\n");
        
        bzero(super_nodes[free_spot], 100);
        strncpy(super_nodes[free_spot], super_node_ip, 100);
        printf("super node saved as %s at %d\n", super_nodes[free_spot], free_spot);
        super_node_heartbeat[free_spot] = 1;
        pthread_mutex_unlock(&super_node_lock);
    }
    else{
        printf("invalid request %d, closing connection\n", request_type);
        close(client_fd);
        free(args);
        pthread_exit(NULL);
    }


    //close the connection
    printf("closing\n");
    close(client_fd);
    free(args);
    pthread_exit(NULL);
}


void *process_heartbeat(void *args){

    int i;
    while(1){
        pthread_mutex_lock(&worker_lock);
        for(i=0; i<MAX_WORKERS; i++){
            if(worker_heartbeat[i] == 1){
                worker_heartbeat[i] = 0;
            }
            else{
                if(workers[i][0] != '\0'){
                    printf("[heartbeat] worker %s at %d inactive, clearing entry in table\n", workers[i], i);
                    workers[i][0] = '\0';
                }
            }
        }
        pthread_mutex_unlock(&worker_lock);
        sleep(HEARTBEAT_WAIT_TIME);
    }
}

void *process_super_node_heartbeat(void *args){
    int i;
    while(1){
        pthread_mutex_lock(&super_node_lock);
        for(i=0; i<MAX_SUPER_NODE; i++){
            if(super_node_heartbeat[i] == 1){
                super_node_heartbeat[i] = 0;
            }
            else{
                if(super_nodes[i][0] != '\0'){
                    printf("[heartbeat] super node %s at %d inactive, clearing entry in table\n", super_nodes[i], i);
                    super_nodes[i][0] = '\0';
                }
            }
        }
        pthread_mutex_unlock(&super_node_lock);
        sleep(HEARTBEAT_WAIT_TIME);
    }
}
