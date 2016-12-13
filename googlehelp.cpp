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

void connect2Nameserver (int iter, char * nsfile, char *nsname, char * nsport) {

    char buf[MAXBUFLEN], *start, *end;    

    findNameserver(nsfile, buf, iter);
    start = buf;
    end = strchr(buf, ':');
    strncpy(nsname, start, end-start);
    nsname[end-start] = '\0';
    start = end + 1;
    strcpy(nsport, start);

    return;
}

void findNameserver (char * filename, char * buf, int iter) {

    ifstream infile;
    int i;
    bool keepLooping;
    char ch;
    string lastLine;

    infile.open(filename);

    keepLooping = true;

    if (infile.is_open()) {
	// Loop to each line in ns file
	for (i = 0; i < iter; ++i) {
	    if(!getline(infile, lastLine)){
		while(!getline(infile, lastLine)){
		    infile.close();
		    infile.open(filename);
		}
	    }
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

    char strVal[MAXBUFLEN], hostname[MAXBUFLEN], hostport[MAXBUFLEN], filename[MAXBUFLEN], sizebuf[32], *start, *end;
    int action, filesize;
    struct addrinfo worktcphints, helphints, *hp;
    int numbytes, worktcpsockfd, helpsockfd;
    size_t endLoc, startLoc;    

#ifdef DEBUG

    cout << "MY HELPER HAS: " << endl;

    cout << ((ChunkParam *)params)->myhelper << endl;
    cout << (((ChunkParam *)params)->currentPos) << endl;
    cout << ((ChunkParam *)params)->searchStr << endl;
    cout << ((ChunkParam *)params)->filesize << endl;

#endif

// START FOR EACH HELPER
    // Get helper IP info
    start = ((ChunkParam *)params)->myhelper;
    end = strchr(((ChunkParam *)params)->myhelper, ':');
    strncpy(hostname, start, end-start);
    hostname[start-end] = '\0';
    start = end + 1;
    strcpy(hostport, start);
    hostport[strlen(start)-1] = '\0';

    action = htonl((int32_t)INDEX);

    createTCPSend(&worktcpsockfd, &worktcphints, hostname, hostport);

    sendTCP(&numbytes, &worktcpsockfd, (void *)&action, sizeof(int32_t));

    recvTCP(&numbytes, &worktcpsockfd, (void *)strVal, MAXBUFLEN);
    strVal[numbytes] = '\0';

    // Get worker IP info
    start = strVal;
    end = strchr(strVal, ':');
    strncpy(hostname, start, end-start);
    hostname[end-start] = '\0';
    start = end + 1;
    strcpy(hostport, start);
    hostport[strlen(start)] = '\0';

    // Create new addrinfo space
    hp = new struct addrinfo;

    // Copy filename
    strcpy(filename, ((ChunkParam *)params)->filename);
    filename[strlen(((ChunkParam *)params)->filename)] = '\0';

    vector<char> ptr (((ChunkParam *)params)->filesize + 1 + MAXBUFLEN, 0);

    //MUTEX HERE
    pthread_mutex_lock(&readFile_lock);
	    
    if (((((ChunkParam *)params)->currentPos)) == string::npos) {
	pthread_mutex_unlock(&readFile_lock);
	pthread_exit(NULL);
    }

    startLoc = (((ChunkParam *)params)->currentPos);
    endLoc = startLoc;

    // Find next occurance of keyword
    endLoc = (*(((ChunkParam *)params)->filePtr)).str().find(((ChunkParam *)params)->searchStr, startLoc+1);

    // If no keyword, must have gotten to the last chunk
    if (endLoc == string::npos) {
	endLoc = ((ChunkParam *)params)->filesize-1;
	(((ChunkParam *)params)->currentPos) = string::npos;
    }

    //MUTEX UNLOCK
    pthread_mutex_unlock(&readFile_lock);

    // resize the ptr buffer to fit only what we need to send
    ptr.resize(endLoc-startLoc+32+1);

    // Convert size to character array
    snprintf(sizebuf, sizeof(sizebuf), "%d", (endLoc-startLoc));
    strncpy(ptr.data(), sizebuf, 32); 

    // Concatenate chunk data to message
    (*(((ChunkParam *)params)->filePtr)).str().copy(ptr.data()+32, endLoc-startLoc, startLoc);
    ptr[endLoc-startLoc+32] =  '\0';

#ifdef DEBUG
    cout << "HostName: " << hostname << "\tPort: " << hostport << endl;
#endif
    // Create a UDP port to send and receive
    createUDPSend(&helpsockfd, &helphints, hp, hostname, hostport);

    if ((endLoc - startLoc) > 10) {
	// Send UDP Message
	sendUDP(&numbytes, &helpsockfd, ptr.data(), endLoc-startLoc+MAXBUFLEN, hp);
    }

    // Close the socket
    close(helpsockfd);

    // TODO: Wait for response
    createUDPSend(&helpsockfd, &helphints, hp, hostname, hostport);
    char doneCode[MAXBUFLEN];
    if (endLoc == (filesize - 1)) {
	strcpy(doneCode, "CODE41");
	doneCode[strlen("CODE41")] = '\0';
    }
    else { 
	strcpy(doneCode, "CODE31");
	doneCode[strlen("CODE31")] = '\0';
    }
    ptr.resize(strlen(doneCode)+32+1);
    snprintf(sizebuf, sizeof(sizebuf), "%d", (strlen(doneCode)));
    strncpy(ptr.data(), sizebuf, 32); 
    strcpy(ptr.data()+32, doneCode);
    ptr[strlen(doneCode)+32] = '\0';
    sendUDP(&numbytes, &helpsockfd, ptr.data(), strlen(doneCode)+32, hp);
    sendUDP(&numbytes, &helpsockfd, filename, strlen(filename), hp);

    close(helpsockfd);

    cout << "DONE" << endl;

    // END
    delete hp;

    pthread_exit(NULL);
}

void sendSort(char * myhelper, char *allhelpers, int numhelpers) {

    char strVal[MAXBUFLEN], hostname[MAXBUFLEN], hostport[MAXBUFLEN], filename[MAXBUFLEN], sizebuf[32], *start, *end;
    int action, filesize;
    struct addrinfo worktcphints, helphints, *hp;
    int numbytes, worktcpsockfd, helpsockfd;
    size_t endLoc, startLoc;    


// START FOR EACH HELPER
    // Get helper IP info
    start = myhelper;
    end = strchr(myhelper, ':');
    strncpy(hostname, start, end-start);
    hostname[start-end] = '\0';
    start = end + 1;
    strcpy(hostport, start);
    hostport[strlen(start)-1] = '\0';

    action = htonl((int32_t)SORT);

    createTCPSend(&worktcpsockfd, &worktcphints, hostname, hostport);

    sendTCP(&numbytes, &worktcpsockfd, (void *)&action, sizeof(int32_t));
    sendTCP(&numbytes, &worktcpsockfd, (void *)&numhelpers, sizeof(int32_t));

    int i;
    for (i = 0; i < numhelpers; ++i) {
	sendTCP(&numbytes, &worktcpsockfd, (void *)(allhelpers+(i*MAXBUFLEN)), MAXBUFLEN);
    }

#if 0
//    recvTCP(&numbytes, &worktcpsockfd, (void *)strVal, MAXBUFLEN);
//    strVal[numbytes] = '\0';

    // Get worker IP info
    start = strVal;
    end = strchr(strVal, ':');
    strncpy(hostname, start, end-start);
    hostname[end-start] = '\0';
    start = end + 1;
    strcpy(hostport, start);
    hostport[strlen(start)] = '\0';

    // Create new addrinfo space
    hp = new struct addrinfo;

    // Copy filename
//    strcpy(filename, ((ChunkParam *)params)->filename);
//    filename[strlen(((ChunkParam *)params)->filename)] = '\0';

//    vector<char> ptr (((ChunkParam *)params)->filesize + 1 + MAXBUFLEN, 0);

    //MUTEX HERE
//    pthread_mutex_lock(&readFile_lock);
	    
//    if (((((ChunkParam *)params)->currentPos)) == string::npos) {
//	pthread_mutex_unlock(&readFile_lock);
//	pthread_exit(NULL);
//    }

//    startLoc = (((ChunkParam *)params)->currentPos);
//    endLoc = startLoc;

    // Find next occurance of keyword
//    endLoc = (*(((ChunkParam *)params)->filePtr)).str().find(((ChunkParam *)params)->searchStr, startLoc+1);

    // If no keyword, must have gotten to the last chunk
//    if (endLoc == string::npos) {
//	endLoc = ((ChunkParam *)params)->filesize-1;
//	(((ChunkParam *)params)->currentPos) = string::npos;
//    }

    //MUTEX UNLOCK
//    pthread_mutex_unlock(&readFile_lock);

    // resize the ptr buffer to fit only what we need to send
//    ptr.resize(endLoc-startLoc+32+1);

    // Convert size to character array
//    snprintf(sizebuf, sizeof(sizebuf), "%d", (endLoc-startLoc));
//    strncpy(ptr.data(), sizebuf, 32); 

    // Concatenate chunk data to message
    (*(((ChunkParam *)params)->filePtr)).str().copy(ptr.data()+32, endLoc-startLoc, startLoc);
    ptr[endLoc-startLoc+32] =  '\0';

#ifdef DEBUG
    cout << "HostName: " << hostname << "\tPort: " << hostport << endl;
#endif
    // Create a UDP port to send and receive
    createUDPSend(&helpsockfd, &helphints, hp, hostname, hostport);

    if ((endLoc - startLoc) > 10) {
	// Send UDP Message
	sendUDP(&numbytes, &helpsockfd, ptr.data(), endLoc-startLoc+MAXBUFLEN, hp);
    }

    // Close the socket
    close(helpsockfd);

    // TODO: Wait for response
    createUDPSend(&helpsockfd, &helphints, hp, hostname, hostport);
    char doneCode[MAXBUFLEN];
    if (endLoc == (filesize - 1)) {
	strcpy(doneCode, "CODE41");
	doneCode[strlen("CODE41")] = '\0';
    }
    else { 
	strcpy(doneCode, "CODE31");
	doneCode[strlen("CODE31")] = '\0';
    }
    ptr.resize(strlen(doneCode)+32+1);
    snprintf(sizebuf, sizeof(sizebuf), "%d", (strlen(doneCode)));
    strncpy(ptr.data(), sizebuf, 32); 
    strcpy(ptr.data()+32, doneCode);
    ptr[strlen(doneCode)+32] = '\0';
    sendUDP(&numbytes, &helpsockfd, ptr.data(), strlen(doneCode)+32, hp);
    sendUDP(&numbytes, &helpsockfd, filename, strlen(filename), hp);

    close(helpsockfd);
#endif

    cout << "DONE" << endl;

    // END
//    delete hp;

    return; //pthread_exit(NULL);
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
	findNameserver(nsfile, buf, iter);

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

	cout << "I Want " << numhelpers << " helpers" << endl;

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

    filesize = getFilesize(((threadParam *)params)->buf);
    vector<char> ptr (filesize + 1, 0);

    infile.open(((threadParam *)params)->buf);

    if (!infile.is_open()) {
	cout << "Error: File Does not Exist" << endl;
	pthread_exit(NULL);
    }

    readBuf << infile.rdbuf();

    char searchStr[10];

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

    ChunkParam chunkinfo;

    i = 0;
    while (1){
	strcpy(chunkinfo.myhelper, myhelpers[i]);
	cout << strlen(myhelpers[i]) << endl;
	chunkinfo.myhelper[strlen(myhelpers[i])] = '\0';
	strcpy(chunkinfo.filename, ((threadParam*)params)->buf);
	chunkinfo.filename[strlen(((threadParam*)params)->buf)] = '\0';
	chunkinfo.filePtr = &readBuf;
	chunkinfo.currentPos = currentPos;
	cout << i << endl;
	strcpy(chunkinfo.searchStr, searchStr);
	chunkinfo.searchStr[strlen(searchStr)] = '\0';
	chunkinfo.filesize = filesize;
	createNewThread(CHNK, (void * )&chunkinfo);
	currentPos = readBuf.str().find(searchStr, currentPos+1);
	if (currentPos == string::npos) {
//	    for (i = 0; i < numhelpers; ++ i) {
//		char ipname[MAXBUFLEN], ipport[MAXBUFLEN], *start, *end;
    // Get worker IP info
//  start = myhelpers[i];
//  end = strchr(strVal, ':');
//  strncpy(ipname, start, end-start);
//  ipname[end-start] = '\0';
//  start = end + 1;
//  strcpy(ipport, start);
//  ipport[strlen(start)] = '\0';
//createUDPSend(&helpsockfd, &helphints, hp, ipname, ipport);
//char doneCode[MAXBUFLEN] = "CODE41";
//		ptr.resize(strlen(doneCode)+32+1);
//		snprintf(sizebuf, sizeof(sizebuf), "%d", (strlen(doneCode)));
//		strncpy(ptr.data(), sizebuf, 32); 
//		strcpy(ptr.data()+32, doneCode);
//		ptr[strlen(doneCode)+32] = '\0';
//		sendUDP(&numbytes, &helpsockfd, ptr.data(), strlen(doneCode)+32, hp);
//
//		close(helpsockfd);
//	    }
	    break;
	}
	i = (i + 1) % numhelpers;
    }
    
    for (i = 0; i < numhelpers; ++i) {
	sendSort(myhelpers[i], myhelpers[0], numhelpers);	
    }

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

void * countCall(void * params) {

    char *pchstart, *pchend, helpers[3];
    char indexfile[MAXBUFLEN];
    threadParam tdparams;

    cout << "BUF: " <<  ((threadParam *)params)->buf << endl;

//    strncpy(helpers, ((threadParam *)params)->buf+5, 2);
//    helpers[2] = '\0';

//    cout << "Helpers: " << helpers << "\t" << atoi(helpers) << endl;

//    tdparams.numhelpers = atoi(helpers); 

//    cout << "stored: " << tdparams.numhelpers << endl;

    pchstart = ((threadParam *)params)->buf+5;
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
	createNewThread(SRCH, (void*)&tdparams);

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
    int iter;
    char nsfile[MAXBUFLEN] = "ns.txt", buf[MAXBUFLEN], *start, *end;


    iplen = htonl(strlen(((threadParam *)params)->myipinfo));

    while (1) {
	if (((threadParam *)params)->keepRunning == 1) {
	    iter = 0;
	    while(createTCPSend(&sockfd, &hints, (((threadParam *)params)->nameserver), (((threadParam *)params)->nameserverport))){
		iter++;
		connect2Nameserver (iter, nsfile, ((threadParam *)params)->nameserver, ((threadParam *)params)->nameserverport);
	    }	
		
	    sendTCP(&numbytes, &sockfd, &beatOpt, sizeof(int32_t));
	    sendTCP(&numbytes, &sockfd, &iplen, sizeof(int32_t));
	    sendTCP(&numbytes, &sockfd, ((threadParam *)params)->myipinfo, ntohl(iplen));
	    close(sockfd);
	    cout << "Heartbeat" << endl;
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
    char nsfile[MAXBUFLEN] = "ns.txt", buf[MAXBUFLEN], *start, *end;
    int iter;

    iplen = htonl(strlen(((threadParam *)params)->myipinfo));

    while (1) {
	if (((threadParam *)params)->keepRunning == 1) {
	    iter = 0;
	    while(createTCPSend(&sockfd, &hints, (((threadParam *)params)->nameserver), (((threadParam *)params)->nameserverport))){
		iter++;
		connect2Nameserver (iter, nsfile, ((threadParam *)params)->nameserver, ((threadParam *)params)->nameserverport);
	    }	
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

void * sendDataHeartbeat(void *params) {

    int32_t beatOpt = htonl((int32_t)DHEARTBEAT);
    int numbytes;
    int sockfd = (((threadParam *)params)->returnfd);
    struct addrinfo hints;
    int32_t iplen;
    char nsfile[MAXBUFLEN] = "ns.txt", buf[MAXBUFLEN], *start, *end;
    int iter;

    iplen = htonl(strlen(((threadParam *)params)->myipinfo));

    while (1) {
	if (((threadParam *)params)->keepRunning == 1) {
	    iter = 0;
	    while(createTCPSend(&sockfd, &hints, (((threadParam *)params)->nameserver), (((threadParam *)params)->nameserverport))){
		iter++;
		connect2Nameserver (iter, nsfile, ((threadParam *)params)->nameserver, ((threadParam *)params)->nameserverport);
	    }	
	    sendTCP(&numbytes, &sockfd, &beatOpt, sizeof(int32_t));
	    sendTCP(&numbytes, &sockfd, &iplen, sizeof(int32_t));
	    sendTCP(&numbytes, &sockfd, ((threadParam *)params)->myipinfo, ntohl(iplen));
	    cout << "DataBeat" << endl;
	    close(sockfd);
	}
	sleep(10);
    }

    pthread_exit(NULL);
}

void * send2Datanode(void * params) {

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
	findNameserver(nsfile, buf, iter);

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

    while (1) {
	createTCPSend(&nssockfd, &nshints, nsname, nsport);

	action = htonl((int32_t)DREQUEST);

	sendTCP(&numbytes, &nssockfd, (void *)&action, sizeof(int32_t));
	recvTCP(&numbytes, &nssockfd, (void *)&numhelpers, sizeof(int32_t));

	numhelpers = ntohl(numhelpers);

	cout << "I get " << numhelpers << " helpers" << endl;
	if (numhelpers > 0) {
	    break;
	}
	else {
	    sleep(2);
	}
	close(nssockfd);
    }

    recvTCP(&numbytes, &nssockfd, (void *)buf, MAXBUFLEN);

    close(nssockfd);

    start = buf;
    end = strchr(buf, ':');
    strncpy(hostname, start, end-start);
    hostname[end-start] = '\0';
    start = end + 1;
    end = strchr(start, ';');
    strncpy(hostport, start, end-start);
    hostport[end-start] = '\0';

    createTCPSend(&worktcpsockfd, &worktcphints, hostname, hostport);

    action = htonl((int32_t)SENDCOUNT);

    sendTCP(&numbytes, &worktcpsockfd, (void *)&action, sizeof(int32_t));
    sendTCP(&numbytes, &worktcpsockfd, (void *)((threadParam *)params)->buf, MAXBUFLEN);

    recvTCP(&numbytes, &worktcpsockfd, buf, MAXBUFLEN);

    cout << "It says: " << buf << endl;

    sendTCP(&numbytes, &(((threadParam *)params)->returnfd), buf, MAXBUFLEN);

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
	    pthread_create(&t, NULL, countCall, param);
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
	case DHEARTBEAT:
	    pthread_create(&t, NULL, sendDataHeartbeat, param);
	    break;
	case SRCH:
	    pthread_create(&t, NULL, send2Datanode, param);
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
