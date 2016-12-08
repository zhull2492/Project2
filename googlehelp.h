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
#define IND 0
#define CNT 1
#define TOP 2

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
