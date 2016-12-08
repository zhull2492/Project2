#include "googlehelp.h"

using namespace std;

int getFilesize(const char *filename) {

    int filesize;

    ifstream in (filename, ios::binary|ios::ate);
    filesize = in.tellg();
    in.close();

    return filesize;
}

void * send2helpers(void * params) {

    int h;
    int filesize, sizeperhelper, totoalSent, sendBuf, numhelpers;
    int status, helpsockfd, numbytes;
    char helperport[MAXBUFLEN], helpername[MAXBUFLEN], sizebuf[32];
    stringstream readBuf;
    ifstream infile;
    struct addrinfo helphints, *helpres, *hp;
    size_t startLoc, endLoc;

    numhelpers = 1;

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
    cout << "My File: \n" << readBuf.str() << endl;
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

    cout << "Start: " << startLoc << endl;

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
#endif
	    char hostname[50] = "oxygen.cs.pitt.edu";
	    char hostport[5] = "5490";

	    // Create a UDP port to send and receive
	    createUDPSend(&helpsockfd, &helphints, hp, hostname, hostport);

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
    pthread_exit(NULL);
}

void * indexCall(void * params) {

    char *pchstart, *pchend;
    char indexfile[MAXBUFLEN];
    threadParam tdparams;

    cout << "BUF: " <<  ((threadParam *)params)->buf << endl;
 
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
	createNewThread(IND, (void*)&tdparams);

	// Need a little pause for accuracy
	usleep (100);
	pchstart = pchend + 1;
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
	default:
	    break;
    }

    return;
}
