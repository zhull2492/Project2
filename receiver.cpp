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

#define MAXBUFLEN 100
#define MAXRECVLEN 1000

using namespace std;

int main () {

    char name[MAXBUFLEN], buf[MAXRECVLEN+1];
    size_t len = MAXBUFLEN;
    struct addrinfo hints, *res, *p;
    struct sockaddr_storage their_addr;
    socklen_t addr_len;
    int status, numbytes, totalReceived, totalSize, receiveBuf;
    int sockfd;

    if (gethostname(name, len) != 0) {
	fprintf(stderr, "gethostname\n");
    }
    else {
	printf("GetHost: %s\n\n", name);
    }

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    if ((status = getaddrinfo(name, "5490", &hints, &res)) != 0) {
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
    }

    return 0;
}
