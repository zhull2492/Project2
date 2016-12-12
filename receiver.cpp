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

#define MAXBUFLEN 100
#define MAXRECVLEN 1000
#define ECHO      0
#define REQUEST   1
#define REGISTER  2
#define HEARTBEAT 3

using namespace std;

#if 0
struct IPInfo{
    char ipaddr[MAXBUFLEN];
    char port[MAXBUFLEN];
};
#endif

//void createTCPSend (int * sockfd, struct addrinfo *hints, char *hostname, char *hostport);
//void sendTCP (int *numbytes, int* sockfd, void* buf, int bytesToSend);
//void getInfo(IPInfo * info, int * sockfd);
//void readLastLine(char * filename, char * buf);
//void *sendHeartbeat(void *params);

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
    word_list * wordindex;
    int iter = 1;

    createTCPRecv(&tcpsockfd, &tcphints, NULL);

    strcpy(nsfile, "ns.txt");

    while (1) {
    readLastLine(nsfile, buf, iter);

    start = buf;
    end = strchr(buf, ':');
    strncpy(nsname, start, end-start);
    nsname[start-end] = '\0';
    cout << "NameServer Info\nIP: " << nsname;
    start = end+1;
    strcpy(nsport, start);
    cout << "\tPort: " << nsport << endl;

    if (!createTCPSend(&nssockfd, &nshints, nsname, nsport)) {
	break;
    }
    iter++;
    }

    getInfo(&myinfo, &tcpsockfd);
    strcpy(strVal, myinfo.ipaddr);
    strVal[strlen(myinfo.ipaddr)] = ':';
    strVal[strlen(myinfo.ipaddr)+1] = '\0';
    strcat(strVal,myinfo.port);


    cout << "IP String: " << strVal << endl;


    request = htonl((int32_t)REGISTER);
    value = htonl((int32_t)strlen(strVal));

    sendTCP(&numbytes, &nssockfd, (void *)&request, sizeof(int32_t));
    sendTCP(&numbytes, &nssockfd, (void *)&value, sizeof(int32_t));
    sendTCP(&numbytes, &nssockfd, (void *)strVal, ntohl(value));

    tparam.returnfd = nssockfd;
    strcpy(tparam.nameserver, nsname);
    tparam.nameserver[strlen(nsname)] = '\0';
    strcpy(tparam.nameserverport, nsport);
    tparam.nameserverport[strlen(nsport)] = '\0';
    strcpy(tparam.myipinfo, strVal);
    tparam.myipinfo[strlen(strVal)] = '\0';
    tparam.keepRunning = 1;

    close(nssockfd);

    createNewThread(HEARTBEAT, (void *)&tparam);

//    listenTCP(&tcpsockfd, TCPBUFFER);

    while (1) {
	acceptTCP(&tcpsockfd, &newfd, &their_tcp_addr, &tcpaddrlen);

//	tparam.keepRunning = 0;

	recvTCP(&numbytes, &newfd, (int32_t *)&action, sizeof(int32_t));//MAXSEND);

//	strncpy(actionstr, buf, 2);
//	action = atoi(actionstr);

	cout << "From GoogleServer: " << ntohl(action) << endl;

//	if (action == INDEX) {
	    createUDPRecv(&sockfd, &hints, NULL);
	    getInfo(&myudpinfo, &sockfd);

	    strcpy(strVal, myudpinfo.ipaddr);
	    strVal[strlen(myudpinfo.ipaddr)] = ':';
	    strVal[strlen(myudpinfo.ipaddr)+1] = '\0';
	    strcat(strVal,myudpinfo.port);

	    cout << "Send Chunk to : " << strVal << endl;

	    sendTCP(&numbytes, &newfd, (void *)strVal, strlen(strVal));

//	    recvUDP();
#if 0
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    if ((status = getaddrinfo(myinfo.ipaddr, "5490", &hints, &res)) != 0) {
	fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
	return 0;
    }

    for (p = res; p != NULL; p = p -> ai_next) {
	
	if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == 1) {
	    perror("receiver: socket");
	    continue;
	}

	if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
	    close(sockfd);
	    perror("receiver: bind");
	    continue;
	}

	break;
    }

    if (p == NULL) {
	fprintf(stderr, "receiver: failed to create socket\n");
	return 0;
    }

    freeaddrinfo(res);
#endif
    addr_len = sizeof(their_addr);

    char totalsizechar [32+1];
    int offset, remaining;
    receiveBuf = MAXRECVLEN; 

//    cout << "Capacity: " << givenData.capacity() << "  " << sizeof(givenData) << endl;

    while(1) {

	totalReceived = 0;
	totalSize = 1;
        vector<char> givenData (100000, 0);

	while (totalReceived < totalSize) {
	     if ((numbytes = recvfrom(sockfd, buf, receiveBuf, 0, (struct sockaddr*)&their_addr, &addr_len)) == -1) {
		perror("receiver: recvfrom");
		exit(1);
	    }

	    if (totalReceived == 0) {
		cout << "NEW" << endl;
		strncpy(totalsizechar, buf, 32);
		totalSize = atoi(totalsizechar);
		cout << "TotalSize: " << totalSize << endl;
		printf("%d\t%f\n", totalSize, totalSize);
//		givenData = new char [(int)totalSize+10];
//		vector<char> newVec (totalSize, 0);
		givenData.resize(totalSize);
		remaining = totalSize;
		offset = 32;
	    }
	    else {
		offset = 0;
	    }
 	    buf [numbytes] = '\0';
	    //givenData[totalReceived] = ' ';
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


	//givenData[totalSize] = '\0';
	cout << endl << "Received: \n\n" << givenData.data() << endl;


//	delete[] givenData;
	if (!strcmp(givenData.data(), "CODE31")) {
	    cout << givenData.data() << endl;
	    break;
	}
	else {
	    wordindex = index_string(givenData.data());
	    word_list *curr = wordindex;
	    while(curr != NULL) {
		cout << curr->word << "\t" << curr->count << endl;
		curr = curr -> next;
	    }
	}
    }

	cout << "Working Preparing to re-register for work" << endl;
//	sleep(10);
	tparam.keepRunning = 1;
//	createNewThread(HEARTBEAT, (void *)&tparam);
	close(newfd);
//	sleep(10);

    }

    return 0;
}
#if 0
void readLastLine(char * filename, char * buf) {

    ifstream infile;
    bool keepLooping;
    char ch;
    char junk;
    string lastLine;

    infile.open(filename);

    keepLooping = true;

    if (infile.is_open()) {
	infile.seekg(-2, ios_base::end);
	while(keepLooping) {
	    infile.get(ch);

	    if ((int)infile.tellg() <= 1) {
		infile.seekg(0);
		keepLooping = false;
	    }
	    else if (ch == '\n') {
		keepLooping = false;
	    }
	    else {
		infile.seekg(-2, ios_base::cur);
	    }
	}

	getline(infile, lastLine);
	lastLine.copy(buf, lastLine.length());
	buf[lastLine.length()] = '\0';
	infile.close();
    }
    else {
	cout << "Error: File Does not Exist" << endl;
    }

    return;
}
#endif
#if 0
void sendTCP (int *numbytes, int* sockfd, void* buf, int bytesToSend) {

    if ((*numbytes= send(*sockfd, buf, bytesToSend, 0)) == -1) {
	perror("Send TCP");
	return;
    }

    return;
}
#endif
#if 0
void getInfo(IPInfo * info, int * sockfd) {

    char hostname[MAXBUFLEN+1];
    size_t len = MAXBUFLEN;    
    struct sockaddr_in sin;
    socklen_t len_s;
    struct in_addr **addrlist;

    cout << "HERE" << endl;

    if (gethostname(hostname, len) != 0) {
	fprintf(stderr, "gethostname\n");
    }
    else {
	printf("GetHost: %s\n\n", hostname);
    }

    // Get IP
    struct hostent *h;
    h = gethostbyname(hostname);

    if (h == NULL) {
	printf("Unable to Get host Name");
	return;
    }

    addrlist = (struct in_addr**)h->h_addr_list;

    strcpy(info->ipaddr, inet_ntoa(*addrlist[0]));
    info->ipaddr[strlen(inet_ntoa(*addrlist[0]))] = '\0';

    len_s = sizeof(sin);

    //Get Socket info
    if (getsockname(*sockfd, (struct sockaddr*)&sin, &len_s) == -1 ) {
	perror("Getsockname");
    }

    snprintf(info->port, sizeof(info->port), "%d", sin.sin_port);

    return;
}
#endif
#if 0
void *sendHeartbeat(void *params) {

    int32_t beatOpt = HEARTBEAT;

    while (1) {
	sleep(10);
	sendTCP(&numbytes, &(((threadParam *)params)->returnfd), &beatOpt, sizeof(int32_t));
    }

    pthread_exit(NULL);
}
#endif
