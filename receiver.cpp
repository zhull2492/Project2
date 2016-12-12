#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include "communication.h"
#include "googlehelp.h"
#include "index.h"
#include "reducer.h"

//#define MAXBUFLEN 100
#define MAX_FILE_NAME 100
#define MAXRECVLEN 1000
#define ECHO      0
#define REQUEST   1
#define REGISTER  2
#define HEARTBEAT 3

using namespace std;

int main () {

    char name[MAXBUFLEN], buf[MAXRECVLEN+1], nsname[MAXBUFLEN], nsport[MAXBUFLEN], nsfile[MAXBUFLEN], strVal[MAXBUFLEN+1], actionstr[MAXBUFLEN];
    char *start, *end;
    size_t len = MAXBUFLEN;
    struct addrinfo hints, nshints, tcphints, *res, *p;
    struct sockaddr_storage their_addr, their_tcp_addr;
    socklen_t addr_len, tcpaddrlen;
    int status, numbytes, totalReceived, totalSize, receiveBuf, action;
    int sockfd, tcpsockfd, nssockfd, newfd;
    int32_t request, value;
    IPInfo myinfo, myudpinfo;
    threadParam tparam;
    word_list * wordindex, *masterlist;
    int iter = 1;

    createTCPRecv(&tcpsockfd, &tcphints, NULL);

    strcpy(nsfile, "ns.txt");

    // Read NS file until we find live nameserver
    while (1) {
	findNameserver(nsfile, buf, iter);

	start = buf;
	end = strchr(buf, ':');
	strncpy(nsname, start, end-start);
	nsname[start-end] = '\0';
	cout << "NameServer Info\nIP: " << nsname;
	start = end+1;
	strcpy(nsport, start);
	cout << "\tPort: " << nsport << endl;

	// Can we connect?
	if (!createTCPSend(&nssockfd, &nshints, nsname, nsport)) {
	    break;
	}
	iter++;
    }

    // Collect my tcp info
    getInfo(&myinfo, &tcpsockfd);
    strcpy(strVal, myinfo.ipaddr);
    strVal[strlen(myinfo.ipaddr)] = ':';
    strVal[strlen(myinfo.ipaddr)+1] = '\0';
    strcat(strVal,myinfo.port);

    cout << "IP String: " << strVal << endl;

    request = htonl((int32_t)REGISTER);
    value = htonl((int32_t)strlen(strVal));

    // Register with NS
    sendTCP(&numbytes, &nssockfd, (void *)&request, sizeof(int32_t));
    sendTCP(&numbytes, &nssockfd, (void *)&value, sizeof(int32_t));
    sendTCP(&numbytes, &nssockfd, (void *)strVal, ntohl(value));

    // Store some variables
    tparam.returnfd = nssockfd;
    strcpy(tparam.nameserver, nsname);
    tparam.nameserver[strlen(nsname)] = '\0';
    strcpy(tparam.nameserverport, nsport);
    tparam.nameserverport[strlen(nsport)] = '\0';
    strcpy(tparam.myipinfo, strVal);
    tparam.myipinfo[strlen(strVal)] = '\0';
    tparam.keepRunning = 1;

    close(nssockfd);

    // Heartbeat with NS
    createNewThread(HEARTBEAT, (void *)&tparam);

    masterlist = NULL;

    // Keep work coming
    while (1) {
	cout << "Waiting for Work" << endl;
	acceptTCP(&tcpsockfd, &newfd, &their_tcp_addr, &tcpaddrlen);

	//TODO: Need to create threads depending on map or reduce

	tparam.keepRunning = 0;

	recvTCP(&numbytes, &newfd, (int32_t *)&action, sizeof(int32_t));

	cout << "Action From GoogleServer: " << ntohl(action) << endl;

	// Index a file
	if (ntohl(action) == INDEX) {
	    //TODO: INDEX THREAD
	    //TODO: JOIN THREAD
	    createUDPRecv(&sockfd, &hints, NULL);
	    getInfo(&myudpinfo, &sockfd);

	    // Save some variables
	    strcpy(strVal, myudpinfo.ipaddr);
	    strVal[strlen(myudpinfo.ipaddr)] = ':';
	    strVal[strlen(myudpinfo.ipaddr)+1] = '\0';
	    strcat(strVal,myudpinfo.port);

	    cout << "Send Chunk to : " << strVal << endl;

	    sendTCP(&numbytes, &newfd, (void *)strVal, strlen(strVal));

	    addr_len = sizeof(their_addr);

	    char totalsizechar [32+1];
	    int offset, remaining;
	    receiveBuf = MAXRECVLEN; 
	    char indexfile[MAXBUFLEN+1];

	    // Wait for chunk
	    while(1) {

		totalReceived = 0;
		totalSize = 1;
	        vector<char> givenData (100000, 0);

		// Recieve chunk
		while (totalReceived < totalSize) {
		     if ((numbytes = recvfrom(sockfd, buf, receiveBuf, 0, (struct sockaddr*)&their_addr, &addr_len)) == -1) {
			perror("receiver: recvfrom");
			exit(1);
		    }

		    if (totalReceived == 0) {
			strncpy(totalsizechar, buf, 32);
			totalSize = atoi(totalsizechar);
			cout << "TotalSize: " << totalSize << endl;
			printf("%d\t%f\n", totalSize, totalSize);
			givenData.resize(totalSize);
			remaining = totalSize;
			offset = 32;
		    }
		    else {
			offset = 0;
		    }
	 	    buf [numbytes] = '\0';

		    if (remaining < MAXBUFLEN) {
			strncpy(givenData.data()+totalReceived, buf + offset, remaining);
		    }
		    else {
			strncpy(givenData.data()+totalReceived, buf + offset, numbytes - offset);
		    }
		    totalReceived += numbytes-offset;
		    remaining -= numbytes - offset;
		    cout << endl <<"Received " << totalReceived << "\tRemain " << remaining << endl;
		}

		cout << endl << "Received: \n\n" << givenData.data() << endl;

		// No more in chunk
		if (!strcmp(givenData.data(), "CODE31")) {
		    recvUDP(&numbytes, &sockfd, buf, receiveBuf, (struct sockaddr *)&their_addr, &addr_len);
		    strcpy(indexfile, buf);
		    indexfile[strlen(buf)] = '\0';
		    cout << indexfile << endl;
		    break;
		}
		else {
		    wordindex = index_string(givenData.data());
		    //TODO: MERGE INDEX
		    masterlist = mergeIndex(masterlist, wordindex);
		    word_list *curr = masterlist;
		    while(curr != NULL) {
	//		cout << curr->word << "\t" << curr->count << endl;
			curr = curr -> next;
		    }
		}
	    }

	    cout << "Working Preparing to re-register for work" << endl;
//	    tparam.keepRunning = 1;
	    close(newfd);
	} // INDEX
	else { //REDUCE
	    cout << "NOW SORT" << endl;
	    int32_t numreducers;
	    int j;
	    char buf[MAXBUFLEN], reducers[MAX_WORKERS][MAXBUFLEN], datanodes[MAX_WORKERS][MAXBUFLEN], datahost[MAXBUFLEN], dataport[MAXBUFLEN];
	    struct addrinfo datahints;

	    recvTCP(&numbytes, &newfd, (int32_t *)&numreducers, sizeof(int32_t));
	    cout << "Reducers: " << numreducers;
	    for (j = 0; j < numreducers; ++j) {
		recvTCP(&numbytes, &newfd, &reducers[j], MAXBUFLEN);
		reducers[j][numbytes] = '\0';
		cout << "Names: " << reducers[j] << endl;
	    }
	    sortListN(masterlist, numreducers);
	    word_list * curr = masterlist;
	    createTCPSend(&nssockfd, &nshints, nsname, nsport);

	    int32_t action = htonl((int32_t)DREQUEST);
	    int32_t numdatanodes;
	    int datasockfd;

	    sendTCP(&numbytes, &nssockfd, (void *)&action, sizeof(int32_t));
	    recvTCP(&numbytes, &nssockfd, (void *)&numdatanodes, sizeof(int32_t));

	    numdatanodes = ntohl(numdatanodes);
	    
	    for (j = 0; j < numdatanodes; ++j) {
		recvTCP(&numbytes, &nssockfd, (void *)&datanodes[j], MAXBUFLEN);
		datanodes[j][numbytes] = '\0';
	    }
	    start = datanodes[0];
	    end = strchr(datanodes[0], ':');
	    strncpy(datahost, start, end-start);
	    datahost[end-start] = '\0';
	    start = end + 1;
	    end = strchr(start, ';');
	    strncpy(dataport, start, end-start);
	    dataport[end-start] = '\0';

	    createTCPSend(&datasockfd, &datahints, datahost, dataport);
	    if (numreducers == 1) {
		cout << "I am only reducer" << endl;
		while(curr->next != NULL) {
		    curr = curr -> next;
		}
	    }
	    else {
		for (j = 0; j < numreducers; ++j) {
		    char redip[MAXBUFLEN];
		    char redport[MAXBUFLEN], *start, *end;
		    start = reducers[j];
		    end = strchr(reducers[j], ':');
		    strncpy(redip, start, end-start);
		    redip[end-start] = '\0';
		    start = end + 1;
		    strcpy(redport, start);
		    redport[strlen(start) -1] = '\0';

		    while (((curr->word[0]-'a') %  numreducers) == j) {
			//TODO: Send to datanode
			cout << curr->word << endl;
			curr = curr -> next;
			if (curr == NULL) {
			    break;
			}
		    }
		}
	    }
	    // TODO: Send done
	    close(newfd);
	    close(datasockfd);
	}
	tparam.keepRunning = 1;
    }

    return 0;
}
