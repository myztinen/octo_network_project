#ifndef CONTROLLER_H
#define CONTROLLER_H
#include "connections.h"


typedef struct thread_params {
    struct sockaddr_in client;
    int socket;
    Meeting_server_list *meeting_server_list;
} Thread_params;


int handle_server(int socket, char *send_buffer, char *rec_buffer,  struct sockaddr_in *server, Meeting_server_list *list);
int handle_client(int socket, char *send_buffer, char *rec_buffer,  struct sockaddr_in *server, Meeting_server_list *list);

void read_meeting_list_message(char *message, Meeting_server_list *list, struct sockaddr_in *client);
static void *thread_handler(void *params);

#endif
