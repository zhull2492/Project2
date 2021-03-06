#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
//#include "googleServer.h"
#include "communication.h"
#include "googlehelp.h"

#define SUPER_REQUEST 4

using namespace std;

void indexRequest(char * filename, int num_files);
void queryRequest(char * query, int num_files);

int main (int argc, char* argv[]) {

    char action[6];
    char filename[500], query[500];
    int i, len;

    if (argc < 3) {
	cout << "Usage: ./googleTest [INDEX Number_Workers PathToFile] [COUNT {query1 ..}]\n" << endl;
	return 0;
    }
 
    strcpy(action, argv[1]);

    if (!strcmp(action, "INDEX")) {
	snprintf(filename, 2, "%d", atoi(argv[2]));
	if (atoi(argv[2]) < 10) {
	    filename[1] = ' ';
	}
	filename[2] = ' ';
	len = 2;
	for (i = 3; i < argc; ++i) {
	    strcpy(filename+len, argv[i]);
	    len += strlen(argv[i]);
	    strcpy(filename+len, ";");
	    len++;
	}
	filename[len+1] = '\0';
	cout << filename << endl;
	indexRequest(filename, argc-3);
    }
    else if (!strcmp(action, "COUNT")) {
	len = 0;
	for (i = 2; i < argc; ++i) {
	    cout << argv[i] << endl;
	    strcpy(query+len, argv[i]);
	    len += strlen(argv[i]);
	    strcpy(query+len, ";");
	    len++;
	}
	query[len+1] = '\0';
	cout << query << endl;
	queryRequest(query, argc-3);
    }
    else {
	cout << "Usage: ./googleTest [INDEX Number_Workers PathToFile] [COUNT {query1 ..}]\n" << endl;
	return 0;
    }

    return 0;
}

void indexRequest(char * filename, int num_files) {

    char sendBuf[105], gServer[MAXBUFLEN], nsname[MAXBUFLEN], nsport[MAXBUFLEN], *start, *end, buf[MAXBUFLEN], nsfile[MAXBUFLEN], servname[MAXBUFLEN], servport[MAXBUFLEN];
    struct addrinfo hints, nshints, *res, *p;
    int status, numbytes, sockfd, nssockfd;
    int request, chk;

    strcpy(nsfile, "ns.txt");

    chk = 1;

    while (1) {
    findNameserver(nsfile, buf, chk);
    

    start = buf;
    end = strchr(buf, ':');
    strncpy(nsname, start, end-start);
    nsname[end-start] = '\0';
    cout << "Nameserver Info\nIP: " << nsname;
    start = end + 1;
    strcpy(nsport, start);
    nsport[strlen(start)] = '\0';
    cout << "\tPort: " << nsport << endl;

    if(!createTCPSend(&nssockfd, &nshints, nsname, nsport)) {
	break;
    }
    chk++;
    }

    request = htonl((int32_t)SUPER_REQUEST);
    
    sendTCP(&numbytes, &nssockfd, (void *)&request, sizeof(int32_t));

    recvTCP(&numbytes, &nssockfd, (void *)gServer, MAXBUFLEN);

    cout << "I got: " << numbytes << "\t" << gServer << endl;

    close(nssockfd);

    start = gServer;
    end = strchr(gServer, ':');
    strncpy(servname, start, end-start);
    servname[end-start] = '\0';
    cout << "Googleserver Info\nIP: " << servname;
    start = end + 1;
    strcpy(servport, start);
    servport[strlen(start)] = '\0';
    cout << "\tPort: " << servport << endl;
    cout << strlen(servname) << "\t" << strlen(servport) << endl;

    createTCPSend(&sockfd, &hints, servname, servport);

    strncpy(sendBuf, "INDEX", 5);
    strncpy(sendBuf+5, filename, strlen(filename));
    sendBuf[5+strlen(filename)] = '\0'; 

    cout << filename << "\t" << strlen(filename) << endl;

    cout << sendBuf << endl;

    if ((numbytes = send(sockfd, sendBuf, 105, 0)) == -1) {//, p->ai_addr, p->ai_addrlen)) == -1 ) {
	perror("googleTest: sending request info");
	exit(1);
    }
 
    return;
}

void queryRequest(char * query, int num_files) {

    char sendBuf[105], gServer[MAXBUFLEN], nsname[MAXBUFLEN], nsport[MAXBUFLEN], *start, *end, buf[MAXBUFLEN], nsfile[MAXBUFLEN], servname[MAXBUFLEN], servport[MAXBUFLEN];
    struct addrinfo hints, nshints, *res, *p;
    int status, numbytes, sockfd, nssockfd;
    int request, chk;

    strcpy(nsfile, "ns.txt");

    chk = 1;

    while (1) {
    findNameserver(nsfile, buf, chk);
    

    start = buf;
    end = strchr(buf, ':');
    strncpy(nsname, start, end-start);
    nsname[end-start] = '\0';
    cout << "Nameserver Info\nIP: " << nsname;
    start = end + 1;
    strcpy(nsport, start);
    nsport[strlen(start)] = '\0';
    cout << "\tPort: " << nsport << endl;

    if(!createTCPSend(&nssockfd, &nshints, nsname, nsport)) {
	break;
    }
    chk++;
    }

    request = htonl((int32_t)SUPER_REQUEST);
    
    sendTCP(&numbytes, &nssockfd, (void *)&request, sizeof(int32_t));

    recvTCP(&numbytes, &nssockfd, (void *)gServer, MAXBUFLEN);

    cout << "I got: " << numbytes << "\t" << gServer << endl;

    close(nssockfd);

    start = gServer;
    end = strchr(gServer, ':');
    strncpy(servname, start, end-start);
    servname[end-start] = '\0';
    cout << "Googleserver Info\nIP: " << servname;
    start = end + 1;
    strcpy(servport, start);
    servport[strlen(start)] = '\0';
    cout << "\tPort: " << servport << endl;
    cout << strlen(servname) << "\t" << strlen(servport) << endl;

    createTCPSend(&sockfd, &hints, servname, servport);

    strncpy(sendBuf, "COUNT", 5);
    strncpy(sendBuf+5, query, strlen(query));
    sendBuf[5+strlen(query)] = '\0'; 

    cout << query << "\t" << strlen(query) << endl;

    cout << sendBuf << endl;

    if ((numbytes = send(sockfd, sendBuf, 105, 0)) == -1) {//, p->ai_addr, p->ai_addrlen)) == -1 ) {
	perror("googleTest: sending request info");
	exit(1);
    }

    recvTCP(&numbytes, &sockfd, (void *)sendBuf, MAXBUFLEN);

    cout << "Found: " << sendBuf << endl;
 
    return;
}
