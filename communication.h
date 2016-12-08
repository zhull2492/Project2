#ifndef _COMMUNICATIONS_
#define _COMMUNICATIONS_
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>

#define DEBUG

#define MAXNAME 100
#define TCPBUFFER 10
#define MAXSEND 1000

void createTCPRecv(int * sockfd, struct addrinfo *hints, const char * port);
void listenTCP(int * sockfd, int bufLen);
void acceptTCP(int * sockfd, int * newfd, struct sockaddr_storage * client_addr, socklen_t * addr_len);
void recvTCP(int * numbytes, int * newfd, char* buf, int recvSize);
void createUDPSend(int *sockfd, struct addrinfo *hints, struct addrinfo *pout, char *hostname, char *hostport);
void sendUDP(int * numbytes, int * sockfd, void *buf, int bytesToSend, struct addrinfo *p);
#endif
