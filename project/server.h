/******************************************************************/
//server-related function declarations
#ifndef _SERVER_H_
#define _SERVER_H_

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <pthread.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/uio.h>
#include <sys/types.h>


#define SERVER_PORT 12345

//concatenates premilinary messages that are to appear before desired result
char* displayResponse(char* respond_to_msg, char* prelimMsg);

//Display errors for invalid entries
char* displayError(char* respond_to_msg);

// Facilitates specific response messages for each valid command  
char* verifyCommand(char* myMsg, int sock, struct sockaddr_storage serverAddr);

// Receive messages from socket
void listen_for_command();

char* serveWeb_HR();

char* serveWeb_Calories();

char* serveWeb_Distance();

char* serveWeb_Steps();

char* serveWeb_setHeight(int height);

char* serveWeb_setWeight(int weight);

char* serveWeb_displayHR();

char* serveWeb_displaySteps();

char* serveWeb_displayStats();

void server_thread_init();

void *server_thread(void *__);

#endif



