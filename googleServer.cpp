#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <sstream>
#include <pthread.h>

#include "communication.h"
#include "googlehelp.h"

#if 0
#define DEBUG

#define MAXBUFLEN 100
#define NUM_THREADS 1
#define IND 0
#define CNT 1
#define TOP 2
#endif

using namespace std;

#if 0
int getFilesize(const char * filename);
void * send2helpers(void * params);
void * indexCall(void * params);
void createNewThread(int option, void * param = NULL);

struct threadParam {
    char buf[MAXBUFLEN];
    int status;
    int returnfd;
};
#endif

int main (int argc, char* argv[]) {

    int filesize;
    int status, sockfd, numbytes, newfd, rc;
    char buf[MAXBUFLEN+5+1];
    char action[5+1];
    size_t len = MAXBUFLEN;
    struct addrinfo hints, *res, *p;
    struct sockaddr_storage client_addr;
    socklen_t addrlen;
    threadParam topParams;

    createTCPRecv(&sockfd, &hints, "6290");

    listenTCP(&sockfd, TCPBUFFER);

    addrlen = sizeof(client_addr);

    while (1) {
	acceptTCP(&sockfd, &newfd, &client_addr, &addrlen);

	recvTCP(&numbytes, &newfd, buf, MAXSEND);

	strncpy(action, buf, 5);
	action[5] = '\0';

//	if (!strcmp(action, "INDEX")) {
	    strncpy(topParams.buf, buf, strlen(buf));
	    topParams.buf[strlen(buf)] = '\0';

#ifdef DEBUG
	    cout << topParams.buf << endl;
#endif
	    topParams.status = 1;
	    topParams.returnfd = newfd;

	    createNewThread(TOP, (void *)&topParams);

#ifdef DEBUG
	    cout << "Waiting for more" << endl;
#endif
//	}
//	else if (!strcmp(action, "COUNT")) {
//	    strncpy(topParams.buf, buf, strlen(buf));
//	    topParams.buf[strlen(buf)] = '\0';
//	    topParams.status = 1;
//	    topParams.returnfd = newfd;
	    
//	    createNewThread(TOP
//	}
	close(newfd);
    }

    return 0;
}
