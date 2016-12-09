// author: Charles Smith <cas275@pitt.edu>
//
// a simple test for the name_server

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

    char str[50];
    sprintf(str, "255.255.255.255:255");

    int sock_fd;

    struct sockaddr_in serv;

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&serv, sizeof(struct sockaddr_in));
    serv.sin_family = AF_INET;
    serv.sin_port = htons(ns_port);

    inet_pton(AF_INET, ns_ip, &(serv.sin_addr));
    connect(sock_fd, (struct sockaddr *) &serv, sizeof(struct sockaddr_in));

    int32_t request = 2; // register

    request = htonl(request);
    printf("after htonl request = %d\n", request);
    write(sock_fd, &request, sizeof(int32_t));
    printf("sent request type (2)\n");
    
    //send the length of the string
    int32_t length = strlen(str);
    printf("length is %d\n", length);
    length = htonl(length);
    write(sock_fd, &length, sizeof(int32_t));
    printf("sent\n");
    length = ntohl(length);
    write(sock_fd, &str, sizeof(char) * length);
    printf("sent %s\n", str);
    printf("test complete, check server\n");
}
