#include "googlehelp.h"

pthread_mutex_t readFile_lock;

using namespace std;

int getFilesize(const char *filename) {

    int filesize;

    ifstream in (filename, ios::binary|ios::ate);
    filesize = in.tellg();
    in.close();

    return filesize;
}

void readLastLine (char * filename, char * buf, int iter) {

    ifstream infile;
    int i;
    bool keepLooping;
    char ch;
    string lastLine;

    infile.open(filename);

    keepLooping = true;

    if (infile.is_open()) {
#if 0
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
#endif
	for (i = 0; i < iter; ++i) {
	    getline(infile, lastLine);
	}
	lastLine.copy(buf, lastLine.length());
	buf[lastLine.length()] = '\0';
	infile.close();
    }
    else {
	cout << "Error: File does not exist" << endl;
    }

    return;
}

void * sendChunk(void * params) {

    char strVal[MAXBUFLEN], hostname[MAXBUFLEN], hostport[MAXBUFLEN], sizebuf[32], *start, *end;
    int action, filesize;
    struct addrinfo worktcphints, helphints, *hp;
    int numbytes, worktcpsockfd, helpsockfd;
    size_t endLoc, startLoc;    

#ifdef DEBUG

    cout << "MY HELPER HAS: " << endl;

    cout << ((ChunkParam *)params)->myhelper << endl;
//    cout << (*(((ChunkParam *)params)->filePtr)).str() << endl;
    cout << (((ChunkParam *)params)->currentPos) << endl;
    cout << ((ChunkParam *)params)->searchStr << endl;
    cout << ((ChunkParam *)params)->filesize << endl;

#endif

    cout << "BEGIN" << endl;
    cout.flush();

// START FOR EACH HELPER
    start = ((ChunkParam *)params)->myhelper;
    end = strchr(((ChunkParam *)params)->myhelper, ':');
    strncpy(hostname, start, end-start);
    hostname[start-end] = '\0';
    start = end + 1;
    strcpy(hostport, start);
    hostport[strlen(start)-1] = '\0';

    cout << "HERE1" << endl;
    cout.flush();

    action = htonl((int32_t)INDEX);

    createTCPSend(&worktcpsockfd, &worktcphints, hostname, hostport);

    cout << "HERE1a" << endl;
    cout.flush();

    sendTCP(&numbytes, &worktcpsockfd, (void *)&action, sizeof(int32_t));

    cout << "HERE1b" << endl;
    cout.flush();

    recvTCP(&numbytes, &worktcpsockfd, (void *)strVal, MAXBUFLEN);
    cout << "got: " << numbytes << endl;
    strVal[numbytes] = '\0';

    cout << "HERE2 " << strVal << endl;
    cout.flush();

    start = strVal;
    end = strchr(strVal, ':');
    strncpy(hostname, start, end-start);
    hostname[end-start] = '\0';
    start = end + 1;
//    end = strchr(start, ';');
    strcpy(hostport, start);
    hostport[strlen(start)] = '\0';

    cout << "HERE3" << endl;
    cout.flush();

    // Create new addrinfo space
    hp = new struct addrinfo;

    vector<char> ptr (((ChunkParam *)params)->filesize + 1, 0);

    cout << "Start Looping" << endl;
    cout.flush();

	    // Create a UDP port to send and receive
//	    createUDPSend(&helpsockfd, &helphints, hp, hostname, hostport);

    // While stil need to send chuncks of file
//    while (endLoc != (((ChunkParam *)params)->filesize-1)) {
	    
	    cout << "HERE4" << endl;
	    cout.flush();

	    //MUTEX HERE
	    pthread_mutex_lock(&readFile_lock);

	    cout << "HERE4b" << endl;
	    cout.flush();
	    
	    if (((((ChunkParam *)params)->currentPos)) == string::npos) {
		cout << "Why Here" << endl;
		pthread_mutex_unlock(&readFile_lock);
		pthread_exit(NULL);
	    }

	    cout << "HERE4c" << endl;
	    cout.flush();

	    startLoc = (((ChunkParam *)params)->currentPos);
	    endLoc = startLoc;

	    // Find next occurance of keyword
	    endLoc = (*(((ChunkParam *)params)->filePtr)).str().find(((ChunkParam *)params)->searchStr, startLoc+1);

	    cout << "HERE4a " << startLoc << "\t" << endLoc << endl;
	    // If no keyword, must have gotten to the last chunk
	    if (endLoc == string::npos) {
		endLoc = ((ChunkParam *)params)->filesize-1;
		(((ChunkParam *)params)->currentPos) = string::npos;
	    }
	    else {
//		(((ChunkParam *)params)->currentPos) = endLoc;
	    }
	    //MUTEX UNLOCK
	    pthread_mutex_unlock(&readFile_lock);
	    cout << "HERE5" << endl;
	    cout.flush();


	    // resize the ptr buffer to fit only what we need to send
	    ptr.resize(endLoc-startLoc+32+1);

	    cout << "HERE6" << endl;
	    cout.flush();
	    // Convert size to character array
	    snprintf(sizebuf, sizeof(sizebuf), "%d", (endLoc-startLoc));
	    strncpy(ptr.data(), sizebuf, 32); 

	    cout << "HERE7" << endl;
	    cout.flush();
	    // Concatenate chunk data to message
	    (*(((ChunkParam *)params)->filePtr)).str().copy(ptr.data()+32, endLoc-startLoc, startLoc);
	    ptr[endLoc-startLoc+32] =  '\0';

	    cout << ptr.data() << endl;

	    cout << "HERE8" << endl;
	    cout.flush();
#ifdef DEBUG
	    cout << "HostName: " << hostname << "\tPort: " << hostport << endl;
	    cout << "Len: " << strlen(hostname) << "\t" << strlen(hostport) << endl;
#endif
	    // Create a UDP port to send and receive
	    createUDPSend(&helpsockfd, &helphints, hp, hostname, hostport);
	    cout << "UDP MADE" << endl;

	    cout << "Size: " << endLoc - startLoc << endl;

	    if ((endLoc - startLoc) > 10) {
		// Send UDP Message
		sendUDP(&numbytes, &helpsockfd, ptr.data(), endLoc-startLoc, hp);
		usleep(250);
	    }

	    // Close the socket
	    close(helpsockfd);

//	    startLoc = endLoc;
//    }
//    }
    // TODO: Wait for response
    createUDPSend(&helpsockfd, &helphints, hp, hostname, hostport);
    char doneCode[MAXBUFLEN] = "CODE31";
    ptr.resize(strlen(doneCode)+32+1);
    snprintf(sizebuf, sizeof(sizebuf), "%d", (strlen(doneCode)));
    strncpy(ptr.data(), sizebuf, 32); 
    strcpy(ptr.data()+32, doneCode);
    ptr[strlen(doneCode)+32] = '\0';
    sendUDP(&numbytes, &helpsockfd, ptr.data(), strlen(doneCode)+32, hp);

    close(helpsockfd);

    cout << "DONE" << endl;

    // END
    delete hp;

    pthread_exit(NULL);
}

void * send2helpers(void * params) {

    int h, i;
    int filesize, sizeperhelper, totoalSent, sendBuf, numhelpers;
    int status, helpsockfd, nssockfd, worktcpsockfd, numbytes, action;
    char helperport[MAXBUFLEN], helpername[MAXBUFLEN], sizebuf[32], nsfile[MAXBUFLEN], buf[MAXBUFLEN], nsname[MAXBUFLEN], nsport[MAXBUFLEN], myhelpers[MAX_WORKERS][MAXBUFLEN], hostname[MAXBUFLEN], hostport[MAXBUFLEN], strVal[MAXBUFLEN], *start, *end;
    stringstream readBuf;
    ifstream infile;
    struct addrinfo helphints, nshints, worktcphints, *helpres, *hp;
    size_t startLoc, endLoc;
    IPInfo workinfo;
    int iter = 1;

    strcpy(nsfile, "ns.txt");
    while (1) {
    readLastLine(nsfile, buf, iter);

    start = buf;
    end = strchr(buf, ':');
    strncpy(nsname, start, end-start);
    nsname[start-end] = '\0';
    start = end + 1;
    strcpy(nsport, start);

    if (!createTCPSend(&nssockfd, &nshints, nsname, nsport)) {
	break;
    }
    iter++;
    }

    close(nssockfd);

    numhelpers = 0;

    while (1) {
    createTCPSend(&nssockfd, &nshints, nsname, nsport);

    numhelpers = htonl((int32_t)(((threadParam *)params)->numhelpers));
    action = htonl((int32_t)REQUEST);

    cout << "I Want " << numhelpers << " or " << ((threadParam *)params)->numhelpers << " helpers" << endl;

    sendTCP(&numbytes, &nssockfd, (void *)&action, sizeof(int32_t));
    sendTCP(&numbytes, &nssockfd, (void *)&numhelpers, sizeof(int32_t));
    recvTCP(&numbytes, &nssockfd, (void *)&numhelpers, sizeof(int32_t));

    numhelpers = ntohl(numhelpers);

    cout << "I get " << numhelpers << " helpers" << endl;
    if (numhelpers > 0) {
	break;
    }
    else {
	sleep(2);
    }
    }

    for (i = 0; i < numhelpers; ++i) {
	recvTCP(&numbytes, &nssockfd, (void *)&myhelpers[i][0], MAXBUFLEN);
	myhelpers[i][numbytes] = '\0';
	cout << "Num: " << numbytes << endl;
	cout << "Helper " << i+1 << ": " << myhelpers[i] << endl;
    }   

#if 0
// START FOR EACH HELPER
    start = myhelpers[0];
    end = strchr(myhelpers[0], ':');
    strncpy(hostname, start, end-start);
    hostname[start-end] = '\0';
    start = end + 1;
    strcpy(hostport, start);
    hostport[strlen(start)-1] = '\0';

    action = htonl((int32_t)INDEX);

    createTCPSend(&worktcpsockfd, &worktcphints, hostname, hostport);

    sendTCP(&numbytes, &worktcpsockfd, (void *)&action, sizeof(int32_t));

    recvTCP(&numbytes, &worktcpsockfd, (void *)strVal, MAXBUFLEN);

    start = strVal;
    end = strchr(strVal, ':');
    strncpy(hostname, start, end-start);
    hostname[start-end] = '\0';
    start = end + 1;
    strcpy(hostport, start);
    hostport[strlen(start)] = '\0';
#endif
//    creatUDPSend(

    filesize = getFilesize(((threadParam *)params)->buf);
    vector<char> ptr (filesize + 1, 0);

    infile.open(((threadParam *)params)->buf);

    if (!infile.is_open()) {
	cout << "Error: File Does not Exist" << endl;
	pthread_exit(NULL);
    }

    readBuf << infile.rdbuf();

    char searchStr[10];

#ifdef DEBUG
//    cout << "My File: \n" << readBuf.str() << endl;
#endif

    startLoc = readBuf.str().find("CHAPTER ");
    strcpy(searchStr, "CHAPTER ");
    searchStr[strlen("CHAPTER ")] = '\0';
    if (startLoc == string::npos) {
	startLoc = readBuf.str().find("SCENE ");
	strcpy(searchStr, "SCENE ");
	searchStr[strlen("SCENE ")] = '\0';
    }
    if (startLoc == string::npos) {
	startLoc = readBuf.str().find("Chapter ");
	strcpy(searchStr, "Chapter ");
	searchStr[strlen("Chapter ")] = '\0';
    }
    if (startLoc == string::npos) {
	startLoc = readBuf.str().find("\n\n");
	strcpy(searchStr, "\n\n");
	searchStr[strlen("\n\n")] = '\0';
    }

    endLoc = startLoc;
    size_t currentPos = startLoc;

    cout << "Start: " << startLoc << endl;

//    pthread_mutex_t searchFile_lock;

    ChunkParam chunkinfo;

//    chunkinfo = new ChunkParam [numhelpers];

//    for (i = 0; i < numhelpers; ++i) {
    i = 0;
    while (1){
	strcpy(chunkinfo.myhelper, myhelpers[i]);
	cout << strlen(myhelpers[i]) << endl;
	chunkinfo.myhelper[strlen(myhelpers[i])] = '\0';
	chunkinfo.filePtr = &readBuf;
	chunkinfo.currentPos = currentPos;
	cout << i << endl;
//	cout << chunkinfo[i].currentPos << "\t" << &startLoc << endl;
	strcpy(chunkinfo.searchStr, searchStr);
	chunkinfo.searchStr[strlen(searchStr)] = '\0';
	chunkinfo.filesize = filesize;
	createNewThread(CHNK, (void * )&chunkinfo);
	currentPos = readBuf.str().find(searchStr, currentPos+1);
	if (currentPos == string::npos) {
	    break;
	}
	i = (i + 1) % numhelpers;
    }
#if 0
    // Create new addrinfo space
    hp = new struct addrinfo;

    // While stil need to send chuncks of file
    while (endLoc != (filesize-1)) {
	    // Find next occurance of keyword
	    endLoc = readBuf.str().find(searchStr, startLoc+1);

	    // If no keyword, must have gotten to the last chunk
	    if (endLoc == string::npos) {
		endLoc = filesize-1;
	    }

	    // resize the ptr buffer to fit only what we need to send
	    ptr.resize(endLoc-startLoc+32+1);

	    // Convert size to character array
	    snprintf(sizebuf, sizeof(sizebuf), "%d", (endLoc-startLoc));
	    strncpy(ptr.data(), sizebuf, 32); 

	    // Concatenate chunk data to message
	    readBuf.str().copy(ptr.data()+32, endLoc-startLoc, startLoc);
	    ptr[endLoc-startLoc+32] =  '\0';

#ifdef DEBUG
	    cout << "Sending " << h << ":\n" << ptr.data()+32 << endl;
	    cout << "HostName: " << hostname << "\tPort: " << hostport << endl;
	    cout << "Len: " << strlen(hostname) << "\t" << strlen(hostport) << endl;
#endif
	    // Create a UDP port to send and receive
	    createUDPSend(&helpsockfd, &helphints, hp, hostname, hostport);

	    cout << "UDP MADE" << endl;

	    cout << "Size: " << endLoc - startLoc << endl;

	    if ((endLoc - startLoc) > 10) {
		// Send UDP Message
		sendUDP(&numbytes, &helpsockfd, ptr.data(), endLoc-startLoc, hp);
	    }

	    // Close the socket
	    close(helpsockfd);

	    startLoc = endLoc;
    }
    // TODO: Wait for response

    // END
    delete hp;
#endif
//    delete[] chunkinfo;

    pthread_exit(NULL);
}

void * indexCall(void * params) {

    char *pchstart, *pchend, helpers[3];
    char indexfile[MAXBUFLEN];
    threadParam tdparams;

    cout << "BUF: " <<  ((threadParam *)params)->buf << endl;

    strncpy(helpers, ((threadParam *)params)->buf+5, 2);
    helpers[2] = '\0';

    cout << "Helpers: " << helpers << "\t" << atoi(helpers) << endl;

    tdparams.numhelpers = atoi(helpers); 

    cout << "stored: " << tdparams.numhelpers << endl;

    pchstart = ((threadParam *)params)->buf+5+2;
    while (strlen(pchstart)) {

	// Get filename
	pchend = strchr(pchstart, ';');

	// Parse out Filename
	strncpy(tdparams.buf, pchstart, pchend-pchstart);
	tdparams.buf[pchend-pchstart] = '\0';

	tdparams.status = 1;

#ifdef DEBUG
	cout << tdparams.buf << endl;
#endif

	// TODO: Check if file already indexed

	// Send file to helpers
	createNewThread(IND, (void*)&tdparams);

	// Need a little pause for accuracy
	usleep (100);
	pchstart = pchend + 1;
    }

    pthread_exit(NULL);
}

void * sendHeartbeat(void *params) {

    int32_t beatOpt = htonl((int32_t)HEARTBEAT);
    int numbytes;
    int sockfd = (((threadParam *)params)->returnfd);
    struct addrinfo hints;
    int32_t iplen;


    iplen = htonl(strlen(((threadParam *)params)->myipinfo));

    while (1) {
	if (((threadParam *)params)->keepRunning == 1) {
	createTCPSend(&sockfd, &hints, (((threadParam *)params)->nameserver), (((threadParam *)params)->nameserverport)); 
	sendTCP(&numbytes, &sockfd, &beatOpt, sizeof(int32_t));
	sendTCP(&numbytes, &sockfd, &iplen, sizeof(int32_t));
	sendTCP(&numbytes, &sockfd, ((threadParam *)params)->myipinfo, ntohl(iplen));
	close(sockfd);
	}
	sleep(10);
    }

    pthread_exit(NULL);
}

void * sendSuperHeartbeat(void *params) {

    int32_t beatOpt = htonl((int32_t)SHEARTBEAT);
    int numbytes;
    int sockfd = (((threadParam *)params)->returnfd);
    struct addrinfo hints;
    int32_t iplen;


    iplen = htonl(strlen(((threadParam *)params)->myipinfo));

    while (1) {
	if (((threadParam *)params)->keepRunning == 1) {
	    cout << "NS: " << (((threadParam *) params)->nameserver) << "\t" << (((threadParam *)params)->nameserverport) << endl;
	createTCPSend(&sockfd, &hints, (((threadParam *)params)->nameserver), (((threadParam *)params)->nameserverport)); 
	sendTCP(&numbytes, &sockfd, &beatOpt, sizeof(int32_t));
	sendTCP(&numbytes, &sockfd, &iplen, sizeof(int32_t));
	sendTCP(&numbytes, &sockfd, ((threadParam *)params)->myipinfo, ntohl(iplen));
	cout << "SuperBeat" << endl;
	close(sockfd);
	}
	sleep(10);
    }

    pthread_exit(NULL);
}

void createNewThread(int option, void * param) {

    pthread_t t;

    switch (option) {
	case IND:
	    pthread_create(&t, NULL, send2helpers, param);
	    if (pthread_join(t, NULL)) {
		cout << "Not able to join" << endl;
		sleep(10);
	    }
	    break;
	case CNT:
	    break;
	case TOP:
	    pthread_create(&t, NULL, indexCall, param);
	    break;
	case HEARTBEAT:
	    pthread_create(&t, NULL, sendHeartbeat, param);
	    break;
	case CHNK:
	    pthread_create(&t, NULL, sendChunk, param);
	    if (pthread_join(t, NULL)) {
		cout << "Not able to join" << endl;
		sleep(1);
	    }
	    break;
	case SHEARTBEAT:
	    pthread_create(&t, NULL, sendSuperHeartbeat, param);
	    break;
	default:
	    break;
    }

    return;
}

void getInfo (IPInfo *info, int * sockfd) {

    char hostname[MAXBUFLEN+1];
    size_t len = MAXBUFLEN;
    struct sockaddr_in sin;
    socklen_t len_s;
    struct in_addr ** addrlist;

    if (gethostname(hostname, len) != 0) {
	fprintf(stderr, "gethostname\n");
    }
    else {
	printf("Get Host: %s\n\n", hostname);
    }

    // GET IP
    struct hostent *h;
    h = gethostbyname(hostname);

    if (h == NULL) {
	printf("Unable to get host name");
	return;
    }

    addrlist = (struct in_addr **) h->h_addr_list;

    strcpy(info->ipaddr, inet_ntoa(*addrlist[0]));
    info->ipaddr[strlen(inet_ntoa(*addrlist[0]))] = '\0';

    len_s = sizeof(sin);

    // Get Socket Info
    if (getsockname(*sockfd, (struct sockaddr*)&sin, &len_s) == -1) {
	perror("getsockname");
    }

    snprintf(info->port, sizeof(info->port), "%d", ntohs(sin.sin_port));

    return;
}
