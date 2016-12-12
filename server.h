// author: Charles Smith
//
// a simple header file for servers

#ifndef SERVER_H
#define SERVER_H

void *process_connection(void *args);
void init_server(char * ip, int  port);

#endif
