#ifndef CONTROLLER_H
#define CONTROLLER_H
#include "connections.h"


typedef struct thread_params {
    struct sockaddr_in client;
    int socket;
    Meeting_server_list *meeting_server_list;
} Thread_params;

/*
 * Initial thread function for the newly created thread. Based on the
 * initial message from the peer, it decides one of the two functions
 * below to handle the communication
 */
static void *thread_handler(void *params);

/*
 * This fuction handles the communication with connected Server
 * application. Once Controller accepts new connection, it creates new
 * thread. If the peer is recognised as Server, this function handles
 * the rest of the communication. 
 */
int handle_server(int socket, char *send_buffer, char *rec_buffer,  struct sockaddr_in *server, Meeting_server_list *list);

/*
 * This fuction handles the communication with connected Client
 * application. Once Controller accepts new connection, it creates new
 * thread. If the peer is recognised as Client, this function handles
 * the rest of the communication. 
 */
int handle_client(int socket, char *send_buffer, char *rec_buffer,  struct sockaddr_in *server, Meeting_server_list *list);

/*
 * Parses the update of meetings from server and updates the list of 
 * meetings on Controller.
 */
void read_meeting_list_message(char *message, Meeting_server_list *list, struct sockaddr_in *client);

#endif
