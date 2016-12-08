// author: Charles Smith
//
// a simple echo test for servers
#include <pthread.h>
#include <stdio.h>
#include <strings.h>
#include <stdlib.h>

void *process_connection(void *args){
    int client_fd = *((int *) args);

    char str[256];
    bzero(str, 256);
    read(client_fd, str, 255); // make sure we keep the '\0' at the end
    printf("read: %s\n", str);
    write(client_fd, str, 256);
    close(client_fd);
    free(args);
    pthread_exit(NULL);

}

void init_server(char * ip, int port){
    printf("%s,%d\n", ip, port);
}
