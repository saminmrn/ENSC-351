#include "server.h"
#include "accel.h"
#include "heartrate.h"
#include "audioCommand.h"
#include "oled.h"

char* beatNames[3] = { "none", "rock", "custom" }; 
static pthread_t serverThreadId;

#define SIZE 1024

char createBuffer(int valueToWrite, char * stringToWrite) {
	char buffer[512];
	sprintf(buffer, "%s %d", stringToWrite, valueToWrite);
}

char* serveWeb_HR() {
	char* respond_to_msg = (char*)malloc(sizeof(char)*SIZE);
	memset(respond_to_msg,0,sizeof(char)*SIZE);
    char buffer[512];
    //get volume and increment 
    sprintf(buffer, "%d", getHeartRate());
    strcat(respond_to_msg, buffer);
    return respond_to_msg;
}

char * serveWeb_Calories() {
	char* respond_to_msg = (char*)malloc(sizeof(char)*SIZE);
	memset(respond_to_msg,0,sizeof(char)*SIZE);
    char buffer[512];
    sprintf(buffer, "%d", getCalories());
    strcat(respond_to_msg, buffer);
    return respond_to_msg;
}

char * serveWeb_Distance() {
	char* respond_to_msg = (char*)malloc(sizeof(char)*SIZE);
	memset(respond_to_msg,0,sizeof(char)*SIZE);
    char buffer[512];
    //get volume and increment 
    sprintf(buffer, "%d", getDistanceInKm());
    strcat(respond_to_msg, buffer);
    return respond_to_msg;
}

char * serveWeb_Steps() {
	char* respond_to_msg = (char*)malloc(sizeof(char)*SIZE);
	memset(respond_to_msg,0,sizeof(char)*SIZE);
    char buffer[512];
    //get volume and increment 
    sprintf(buffer, "%d", getSteps());
    strcat(respond_to_msg, buffer);
    return respond_to_msg;
}

/// setters
char * serveWeb_setHeight(int height) {
	char* respond_to_msg = (char*)malloc(sizeof(char)*SIZE);
	memset(respond_to_msg,0,sizeof(char)*SIZE);
    char buffer[512];
    //get volume and increment 
	setHeight(height);
    sprintf(buffer, "success");
    strcat(respond_to_msg, buffer);
    return respond_to_msg;
}

char * serveWeb_setWeight(int weight) {
	char* respond_to_msg = (char*)malloc(sizeof(char)*SIZE);
	memset(respond_to_msg,0,sizeof(char)*SIZE);
	char buffer[512];
	sprintf(buffer, "success");
	setWeight(weight);
	strcat(respond_to_msg, buffer);
    return respond_to_msg;
}

// send to display 
char * serveWeb_displayHR() {
	char* respond_to_msg = (char*)malloc(sizeof(char)*SIZE);
	memset(respond_to_msg,0,sizeof(char)*SIZE);
    char buffer[512];
	// controlTrackerMode(3);
	displayHeartRate(getHeartRate());
    sprintf(buffer, "success");
    strcat(respond_to_msg, buffer);
    return respond_to_msg;
}

char * serveWeb_displaySteps() {
	char* respond_to_msg = (char*)malloc(sizeof(char)*SIZE);
	memset(respond_to_msg,0,sizeof(char)*SIZE);
    char buffer[512];
	controlTrackerMode(2);
    //get volume and increment 
    sprintf(buffer, "success");
    strcat(respond_to_msg, buffer);
    return respond_to_msg;
}

// char * serveWeb_displayTimer(int weight) {
// 	char* respond_to_msg = (char*)malloc(sizeof(char)*SIZE);
// 	memset(respond_to_msg,0,sizeof(char)*SIZE);
//     char buffer[512];
// 	setWeight(weight);
//     //get volume and increment 
//     sprintf(buffer, "success");
//     strcat(respond_to_msg, buffer);
//     return respond_to_msg;
// }

char * serveWeb_displayStats() {
	char* respond_to_msg = (char*)malloc(sizeof(char)*SIZE);
	memset(respond_to_msg,0,sizeof(char)*SIZE);
    char buffer[512];
	controlTrackerMode(4);
    //get volume and increment 
    sprintf(buffer, "success");
    strcat(respond_to_msg, buffer);
    return respond_to_msg;
}


char* verifyCommand(char* myMsg, int sock, struct sockaddr_storage serverAddr)
{
	// char* respond_to_msg = (char*)malloc(sizeof(char)*SIZE);
	printf("Listening...\n");
    printf("this is the message %s", myMsg);
	//conditionals for all commands
	char* token;
    token = strtok(myMsg, " ");
	if(token != NULL) {
		if (strcmp(token, "setHeight") == 0) {
			token = strtok(NULL, " ");
			printf("gsetting height %d \n", token);
			// set height
			return serveWeb_setHeight(atoi(token));
		}

		if (strcmp(token, "setWeight") == 0) {
			token = strtok(NULL, " ");
			// setweight
			printf("setting weight %s \n", token);
			printf("%d",atoi(token));
			return serveWeb_setWeight(atoi(token));
		}
	}

	if(strcmp(myMsg, "getHeartRate\n") == 0)
	{
		printf("getting heartrate \n");
        return serveWeb_HR();
	}

	else if(strcmp(myMsg, "getCalories\n") == 0) {
		printf("getting calories! \n");
		return serveWeb_Calories();
	}

	else if(strcmp(myMsg, "getSteps\n") == 0) {
		printf("getting steps \n");
		return serveWeb_Steps();
	}

	else if(strcmp(myMsg, "getDistance\n") == 0) {
		printf("getting distance \n");
		return serveWeb_Distance();
	}

	else if(strcmp(myMsg, "displaySteps\n") == 0) {
		printf("displayinh steps \n");
		return serveWeb_displaySteps();
	}

	else if(strcmp(myMsg, "displayHeartRate\n") == 0) {
		printf("displayi g heartrate \n");
		return serveWeb_displayHR();
	}

	else if(strcmp(myMsg, "displayStats\n") == 0) {
		printf("displaying heartrate \n");
		return serveWeb_displayStats();
	}
	
	return NULL;
}

void listen_for_command()
{
	printf("thread has been initialized");
	int sock = socket(AF_INET, SOCK_DGRAM, 0);       //socket initailization
	struct sockaddr_storage serverAddr;
	char myMsg[1024];
	_Bool stopping = false;
	int b;

	//binding 
	struct sockaddr_in sockName;
	memset((char*)&sockName, 0, sizeof(sockName));
	sockName.sin_family = AF_INET;
	sockName.sin_port = htons(SERVER_PORT);
	sockName.sin_addr.s_addr = htonl(INADDR_ANY);
	socklen_t serverAddrSize;

	if(bind(sock, (struct sockaddr *)&sockName, sizeof(sockName)))
	{
		perror("binding failed");
	}
    else {
        printf("connected~");
    }
	
	serverAddrSize = sizeof(serverAddr);
	
	//send msg to server
	while((b = recvfrom(sock, myMsg, 1024, 0, (struct sockaddr *)&serverAddr, &serverAddrSize)) > 0 && !stopping)
	{
		myMsg[b] = '\0';
        printf("%s \n", myMsg);
		printf("Message break: %d\n", myMsg[b]);
		char* response = verifyCommand(myMsg, sock, serverAddr);
		sendto(sock, response, 1024, 0, (struct sockaddr *)&serverAddr, serverAddrSize);
		free(response);
	}
	
	close(sock);
}

void *server_thread(void *__) {
	printf("entering the server thread");
	while(true) {
		listen_for_command();
	}
}

void server_thread_init() {
	pthread_create(&serverThreadId, NULL, server_thread, NULL);
}