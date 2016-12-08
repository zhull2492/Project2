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

using namespace std;

void indexRequest(char * filename, int num_files);

int main (int argc, char* argv[]) {

    char action[6];
    char filename[500], query[500];
    int i, len;

    if (argc < 3) {
	cout << "Usage: ./googleTest [INDEX PathToFile] [COUNT {query1 ..}]\n" << endl;
	return 0;
    }
 
    strcpy(action, argv[1]);

    if (!strcmp(action, "INDEX")) {
	len = 0;
	for (i = 2; i < argc; ++i) {
	    strcpy(filename+len, argv[i]);
	    len += strlen(argv[i]);
	    strcpy(filename+len, ";");
	    len++;
	}
	filename[len+1] = '\0';
	cout << filename << endl;
	indexRequest(filename, argc-2);
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
    }
    else {
	cout << "Usage: ./googleTest [INDEX PathToFile] [COUNT {query1 ..}]\n" << endl;
	return 0;
    }

    return 0;
}

void indexRequest(char * filename, int num_files) {

    char sendBuf[105];
    struct addrinfo hints, *res, *p;
    int status, numbytes, sockfd;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if ((status = getaddrinfo("rhenium.cs.pitt.edu", "6290", &hints, &res)) != 0) {
	fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
	return;
    }

    for (p = res; p != NULL; p = p-> ai_next) {
	if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
	    perror("googleTest: socket");
	    continue;
	}

	if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
	    close(sockfd);
	    perror("client: bind");
	}

	break;
    }

    if (p == NULL) {   
	fprintf(stderr, "googleTest: failed to create socket\n");
	return;
    }

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
