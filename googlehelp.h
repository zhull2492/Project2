#ifndef _GOOGLEHELP_
#define _GOOGLEHELP_
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <sstream>
#include <pthread.h>

#include "communication.h"

#define DEBUG

#define MAXBUFLEN 200
#define NUM_THREADS 1
#define MAX_WORKERS 16
#define IND 0
#define CNT 1
#define TOP 2
#define REQUEST 1
#define HEARTBEAT 3
#define SHEARTBEAT 6
#define DHEARTBEAT 9
#define DREQUEST 7
#define CHNK 4
#define INDEX 1
#define SORT 2
#define DREGISTER 8
#define ACCEPTNEW 0
#define SENDCOUNT 1
#define SRCH 10

struct threadParam {
    char buf[MAXBUFLEN];
    int status;
    int returnfd;
    int keepRunning;
    int numhelpers;
    char myport[MAXBUFLEN];
    char myipinfo[MAXBUFLEN];
    char nameserver[MAXBUFLEN];
    char nameserverport[MAXBUFLEN];
};

struct ChunkParam{
    char myhelper[MAXBUFLEN];
    char filename[MAXBUFLEN];
    std::stringstream *filePtr;
    size_t currentPos;
    char searchStr[MAXBUFLEN];
    int filesize;
};

struct IPInfo {
    char ipaddr[MAXBUFLEN];
    char port[MAXBUFLEN];
};

//pthread_mutex_t readFile_lock;

int getFilesize(const char * filename);
void connect2Nameserver (int iter, char *nsfile, char *nsname, char *nsport);
void findNameserver(char * filename, char * buf, int iter);
void * sendChunk(void * params);
void * send2helpers(void * params);
void * indexCall(void * params);
void * sendHeartbeat(void *params);
void * sendSuperHeartbeat(void *params);
void * sendDataHeartbeat(void *params);
void createNewThread(int option, void * param = NULL);
void getInfo(IPInfo *info, int * sockfd);

#endif
