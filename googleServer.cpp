#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <sstream>
#include <pthread.h>

#include "communication.h"
#include "googlehelp.h"

#define SUPER_REGISTER 5
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
    int status, sockfd, nssockfd, numbytes, newfd, rc, value, request;
    char buf[MAXBUFLEN+5+1], nsname[MAXBUFLEN], nsport[MAXBUFLEN], nsfile[MAXBUFLEN], strVal[MAXBUFLEN], *start, *end;
    char action[5+1];
    size_t len = MAXBUFLEN;
    struct addrinfo hints, nshints, *res, *p;
    struct sockaddr_storage client_addr;
    socklen_t addrlen;
    threadParam topParams, tparam;
    IPInfo myinfo;
    int iter = 1;

    createTCPRecv(&sockfd, &hints, NULL);

    getInfo(&myinfo, &sockfd);
    strcpy(strVal, myinfo.ipaddr);
    strVal[strlen(myinfo.ipaddr)] = ':';
    strVal[strlen(myinfo.ipaddr)+1] = '\0';
    strcat(strVal, myinfo.port);

    strcpy(nsfile, "ns.txt");
    while(1) {
	findNameserver(nsfile, buf, iter);

	start = buf;
	end = strchr(buf, ':');
	strncpy(nsname, start, end-start);
	cout << "Nameserver Info\nIP: " << nsname;
	start = end + 1;
	strcpy(nsport, start);
	cout << "\tPort: " << nsport << endl;

	if(!createTCPSend(&nssockfd, &nshints, nsname, nsport)) {
	    break;
        }
	iter++;
    }

    cout << "IP String: " << strVal << endl;
    request = htonl((int32_t)SUPER_REGISTER);
    value = htonl((int32_t)strlen(strVal));

    sendTCP(&numbytes, &nssockfd, (void *)&request, sizeof(int32_t));
    sendTCP(&numbytes, &nssockfd, (void *)&value, sizeof(int32_t));
    sendTCP(&numbytes, &nssockfd, (void *)strVal, ntohl(value));

    tparam.returnfd = nssockfd;
    strcpy(tparam.nameserver, nsname);
    tparam.nameserver[strlen(nsname)] = '\0';
    cout << tparam.nameserver << endl;
    strcpy(tparam.nameserverport, nsport);
    tparam.nameserverport[strlen(nsport)] = '\0';
    strcpy(tparam.myipinfo, strVal);
    cout << tparam.nameserverport << endl;
    tparam.myipinfo[strlen(strVal)] = '\0';
    tparam.keepRunning = 1;

    createNewThread(SHEARTBEAT, &tparam);

    addrlen = sizeof(client_addr);

    while (1) {
	cout << "Waiting for Connection..." << endl;
	acceptTCP(&sockfd, &newfd, &client_addr, &addrlen);

	recvTCP(&numbytes, &newfd, (void *)buf, MAXSEND);

	strncpy(action, buf, 5);
	action[5] = '\0';

	if (!strcmp(action, "INDEX")) {
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
	}
	else if (!strcmp(action, "COUNT")) {
	    cout << "CoUNT" << endl;
	    strncpy(topParams.buf, buf, strlen(buf));
	    topParams.buf[strlen(buf)] = '\0';
	    topParams.status = 1;
	    topParams.returnfd = newfd;
	    
	    createNewThread(CNT, (void *)&topParams);
	}
	close(newfd);
    }

    return 0;
}
