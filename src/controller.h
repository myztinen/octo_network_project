#ifndef CONTROLLER_H
#define CONTROLLER_H
#include "connections.h"

typedef struct meeting {
    char meetingName[20];
    char serverName[20];
    struct sockaddr_in meeting_server;
    struct sockaddr_in meeting_address;
    int participant_Amount;
    struct meeting *next;
    } Meeting;

    
typedef struct meeting_list{
    int amount;
    Meeting *head;
    } Meeting_list;

typedef struct thread_params {
    struct sockaddr_in client;
    int socket;
} Thread_params;

int handle_server(int socket, char *buffer);

int handle_client(int socket, char *buffer);

#endif
