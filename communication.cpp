#include "communication.h"

void createTCPRecv(int * sockfd, struct addrinfo *hints, const char * port) {

    char name[MAXNAME];
    size_t len = MAXNAME;
    struct addrinfo *res, *p;
    int status;

    if (gethostname(name, len) != 0) {
	fprintf(stderr, "gethostname\n");
    }
    else {
	printf("GetHost: %s\n\n", name);
    }

    memset(hints, 0 , sizeof(struct addrinfo));
    hints->ai_family = AF_INET;
    hints->ai_socktype = SOCK_STREAM;

    if ((status = getaddrinfo(name, port, hints, &res)) != 0) {
	fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
	return;
    }

    for (p = res; p != NULL; p = p -> ai_next) {
	if ((*sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == 1) {
	    perror("googleServer: socket");
	    continue;
	}

	if (bind(*sockfd, p->ai_addr, p->ai_addrlen) == -1) {
	    close(*sockfd);
	    perror("googleServer: bind");
	    continue;
	}
	
	break;
    }

    if (p == NULL) {
	fprintf(stderr, "googleServer: failed to create socket\n");
	return;
    }

    freeaddrinfo(res);

    return;
}

void listenTCP(int * sockfd, int bufLen){

    int status;

    if ((status = listen(*sockfd, TCPBUFFER)) == -1) {
	perror("googleServer: Listen");
	exit(1);
    }

    return;
}

void acceptTCP(int * sockfd, int * newfd, struct sockaddr_storage * client_addr, socklen_t * addr_len) {

	*newfd = accept(*sockfd, (struct sockaddr*)client_addr, addr_len);
	if (*newfd == -1) {
	    perror("googleServer: accept");
	}

    return;
}

void recvTCP(int * numbytes, int * newfd, char* buf, int recvSize) {

    if ((*numbytes = recv(*newfd, buf, recvSize, 0)) == -1) {
	perror("googleServer: recv");
	exit(1);
    }

    return;
}

void createUDPSend(int * sockfd, struct addrinfo *hints, struct addrinfo *pout, char *hostname, char *hostport) {

    int status;
    struct addrinfo *res, *p;

    // Setup address struct for helper
    memset(hints, 0, sizeof(struct addrinfo));
    hints->ai_family = AF_INET;
    hints->ai_socktype = SOCK_DGRAM;

    // Get helper info
    if ((status = getaddrinfo(hostname, hostport, hints, &res)) != 0) {
	fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
	return;
    }

    // Create a socket for helper
    for (p = res; p != NULL; p = p->ai_next) {
	if ((*sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
	    perror("googleServer: helpsocket");
	    continue;
	}
	break;
    }

    // Make sure socket is made
    if (p == NULL) {
	fprintf(stderr, "googleServer: failed to create helpsocket\n");
	return;
    }

    memcpy(pout, p, sizeof(struct addrinfo));

    freeaddrinfo(res);

    return;
}

void sendUDP(int * numbytes, int * sockfd, void *buf, int bytesToSend, struct addrinfo *p) {

    int totalSent = 0;

    while (totalSent < bytesToSend) {
	if ((*numbytes = sendto(*sockfd, (uint8_t *)buf + totalSent, MAXSEND, 0, p->ai_addr, p->ai_addrlen)) == -1) {
	    perror("googleServer: sending file chunk");
	    exit(1);
	}
	usleep(250);
	totalSent += *numbytes;
    }

    return;
}
