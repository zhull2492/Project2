#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

int main(void){

    int ns_port = 0;
    char* ns_ip = NULL;

    FILE * ns_fp;
    char * line;
    char ns_str[256];
    size_t len = 0;

    ns_fp = fopen("ns.txt", "r");
    if(ns_fp == NULL){
        printf("no ns.txt in current dir\n");
        return;
    }
    getline(&line, &len, ns_fp);
    strncpy(ns_str, line, 255);
    ns_str[255] = '\0';
    ns_ip = strtok(ns_str,":");
    if(ns_ip == NULL){
        printf("ns_ip is NULL\n");
        return;
    }
    char * tmp = strtok(NULL, "");
    ns_port = atoi(tmp);
    if(ns_port <= 0){
        printf("ns_port invalid: %d\n", ns_port);
        return;
    }
    close(ns_fp);
    printf("name_server location is: %s:%d\n",ns_ip, ns_port);

    int sock_fd;

    struct sockaddr_in serv;

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&serv, sizeof(struct sockaddr_in));
    serv.sin_family = AF_INET;
    serv.sin_port = htons(ns_port);

    inet_pton(AF_INET, ns_ip, &(serv.sin_addr));
    connect(sock_fd, (struct sockaddr *) &serv, sizeof(struct sockaddr_in));

    int32_t request = 1; // request

    printf("request 1 test\n");
    request = htonl(request);
    write(sock_fd, &request, sizeof(int32_t));
    printf("sent request type (1)\n");

    int32_t num_requested = 1;
    num_requested = htonl(num_requested);
    write(sock_fd, &num_requested, sizeof(int32_t));

    int32_t num_to_rec = 0;
    read(sock_fd, &num_to_rec, sizeof(int32_t));
    num_to_rec = ntohl(num_to_rec);
    printf("number of workers to recieve %d\n", num_to_rec);

    if(num_to_rec == 0){
        printf("no workers available.\n");
        return;
    }
    else if(num_to_rec < 0){
        printf("invalid number of workers\n");
        return;
    }
    else if(num_to_rec >1){
        printf("recieving more than expected\n");
    }
    char worker_str[102];
    bzero(worker_str, 102);
    read(sock_fd, &worker_str, sizeof(char) * 101);
    worker_str[101] = '\0';

    printf("worker string %s\n", worker_str);


}
