#ifndef PROJECT_DEFS_H
#define PROJECT_DEFS_h


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


extern const char *HELLO;
extern const char *ACK;
extern const char *NACK;

extern const char *IAMCLIENT;
extern const char *GETDISCUSSIONS;
extern const char *CREATENEWMEETING_CLIENT;
extern const char *GETLISTOFPARTICIPANTS;
extern const char *TALK;
extern const char *TALKTO;

extern const char *IAMSERVER;
extern const char *CONNECTIONINFORMATION;
extern const char *LISTOFMEETINGS_SERVER;
extern const char *MEETINGTERMINATED;

extern const char *CREATENEWMEETING_CONTROLLER;
extern const char *LISTOFMEETINGS_CONTROLLER;

#endif

