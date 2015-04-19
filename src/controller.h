#ifndef CONTROLLER_H
#define CONTROLLER_H
#include "connections.h"

typedef struct meeting {
    char meeting_topic[20];
    char meeting_id[10];
    struct sockaddr_in meeting_address;
    int participant_amount;
    struct meeting *next;
    } Meeting;

    
typedef struct meeting_list{
    int amount;
    Meeting *head;
    } Meeting_list;

typedef struct thread_params {
    struct sockaddr_in client;
    int socket;
    Meeting_list *meeting_list;
} Thread_params;

int add_meeting_to_list(Meeting_list *list, Meeting *new_meeting);

int handle_server(int socket, char *buffer);

int handle_client(int socket, char *buffer);

#endif
