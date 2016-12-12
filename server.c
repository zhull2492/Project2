// author: Charles Smith <cas275@pitt.edu>
//
// a basic threaded sever implementation

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <strings.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "server.h"

#define THREAD_COUNT 30


int main(){


    // create socket
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd <0){
        printf("Unable to create the socket, returned: %d\n", sock_fd);
        exit(1);
    }
    printf("created socket\n");

    
    //bind the socket
    struct sockaddr_in serv;
    bzero( (char *) &serv, sizeof(serv)); // set to 0
 
    serv.sin_family      = AF_INET;
    serv.sin_addr.s_addr = INADDR_ANY;
    serv.sin_port        = 0;
 
    int res = bind(sock_fd, (struct sockaddr *) &serv, sizeof(serv));
 
    if(res < 0){
        printf("unable to bind, returned: %d\n", res);
        exit(1);
    }
    printf("bound socket\n");
    
    // find what port/IP/etc. we're at
    struct sockaddr_in test;
    int len = sizeof(test);
    int len2 = len;
    int failed = 0;

    res = getsockname(sock_fd, (struct sockaddr *) &test, &len);

    if(res < 0){
        printf("cannot getsockname(), returned: %d\n", res);
        failed = 1;
    }
 
    if(len > len2){
        printf("getsockname returned an unexpectedly large struct\nexpected: %d\ngot: %d\n",len, len2);
        failed = 1;
    }

    if(failed){
        exit(1);
    }


    // start listening --Important part
    res = listen(sock_fd, 5); //listen, with at most 5 in queue.  
    if(res < 0){
        printf("could not listen, returned: %d\n", res);
        exit(1);
    }
    printf("listening\n");
    
    
    // find host name
    char hostname[1024];
    hostname[1023] = '\0';
    res = gethostname(hostname,1023);
    if(res < 0){
        printf("could not get hostname, returned: %d\n", res);
        exit(1);
    }
    // printf("hostname: %s \n", hostname);

    // find IP from hostname
    struct hostent* h;
    h = gethostbyname(hostname);
    if(h == NULL){
        printf("could not gethostbyname, returned: NULL\n");
        exit(1);
    }
    struct in_addr **addr_list;
    addr_list = (struct in_addr **) h->h_addr_list;

    // process ip and port of server
    init_server(inet_ntoa(*addr_list[0]), ntohs(test.sin_port));


    // start accepting clients
    pthread_t threads[THREAD_COUNT];
    int i;
    int client_fd;
    while(1){
        for(i=0; i<THREAD_COUNT; i++){
            
            int * client_fd = malloc(sizeof(int));
            *client_fd = accept(sock_fd, NULL, NULL);
            
            printf("connected to client\n");
            // create a thread to handle clients
            pthread_create(&(threads[i]), NULL, process_connection, (void *) client_fd);
        }
    }

    return 0;
}
