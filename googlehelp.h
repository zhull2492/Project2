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

#define MAXBUFLEN 100
#define NUM_THREADS 1
#define MAX_WORKERS 16
#define IND 0
#define CNT 1
#define TOP 2
#define REQUEST 1
#define HEARTBEAT 3
#define SHEARTBEAT 6
#define CHNK 4
#define INDEX 1

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
void readLastLine(char * filename, char * buf, int iter);
void * sendChunk(void * params);
void * send2helpers(void * params);
void * indexCall(void * params);
void * sendHeartbeat(void *params);
void * sendSuperHeartbeat(void *params);
void createNewThread(int option, void * param = NULL);
void getInfo(IPInfo *info, int * sockfd);

#endif
