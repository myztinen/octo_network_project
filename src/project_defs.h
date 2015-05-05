#ifndef PROJECT_DEFS_H
#define PROJECT_DEFS_h
/*
 * This file contains all included system header files and data 
 * data structures that the program is using. also constants that
 * are used in different appllications are stored here.
 */

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <ctype.h>
#include <pthread.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>

#define BUFFER_LENGTH 1000

extern const char *HELLO;
extern const char *ACK;
extern const char *NACK;
extern const char *QUIT;

extern const char *IAMCLIENT;
extern const char *GETDISCUSSIONS;
extern const char *CREATENEWMEETING_CLIENT;
extern const char *GETLISTOFPARTICIPANTS;
extern const char *TALK;
extern const char *TALKTO;

extern const char *IAMSERVER;
extern const char *MEETINGINFORMATION;
extern const char *LISTOFMEETINGS_SERVER;
extern const char *MEETINGTERMINATED;

extern const char *CREATENEWMEETING_CONTROLLER;
extern const char *LISTOFMEETINGS_CONTROLLER;
extern const char *HELPTEXT;

 struct meeting {
    char meeting_topic[20];
    char meeting_id[10];
    int participant_amount;
    struct meeting *next;
    int port;
    };

    
 struct meeting_server{
    char server_id[10];
    struct sockaddr_in server_address;
    int amount_of_meetings;
    struct meeting_server *next;
    struct meeting *head;
    int socket;
    };


 struct meeting_server_list {
    pthread_mutex_t list_mutex;
    int amount;
    struct meeting_server *head;
    };

    
typedef struct meeting Meeting;
typedef struct meeting_server Meeting_server;
typedef struct meeting_server_list Meeting_server_list;

struct client_state  {
    struct sockaddr_in connection;
    int my_socket;
    int connected;
    char *client_id;
    
};

typedef struct client_state Client_state; 



#endif

