#ifndef CONNECTIONS_H
#define CONNECTIONS_H

#include "project_defs.h"

int safe_write(int socket, const char *buffer, int len);
int safe_read(int socket, char *buffer, int len);
/*
 * This function opens a TCP connection given ip address and port
 * and returns the connection socket value.
 */
int connect_to(char *ip_address, int port, struct sockaddr_in *server_addr);
/*
 * This function opens a TCP port to listen with given port
 * and returns the socket value to that connection.
 */
int tcp_listen(int port, struct sockaddr_in *serv_addr);
/*
 * This function creates IAMSERVER message that server applications
 * use when connecting to Controller applications
 */
int create_IAMSERVER_message(char *buffer, int buffer_len, const char *server_id);
/*
 * This function creates IAMCLIENT message that client applications
 * use when connecting to Controller applications
 */
int create_IAMCLIENT_message(char *buffer, int buffer_len, const char *server_id);
/*
 * This function creates LISTOFMEETINGS_SERVER message that server 
 * applications use when connecting to Controller applications. The
 * message contains all meetings that are currently taking place in the
 * server.
 */
int create_LISTOFMEETINGS_SERVER_message(char *buffer, int buffer_len, Meeting_server *list);
/*
 * This function creates CREATENEWMEETING_CLIENT message that client 
 * applications use to create new meetings. The Client sends the message
 * to the Controller, who in turn selects proper server to create
 * the new meeting.
 */
int create_CREATENEWMEETING_CLIENT_message(char *buffer, int buffer_len,  char *topic);
/*
 * This function creates CREATENEWMEETING_CONTROLLER message that 
 * controller applications use to create new meetings. The Controller 
 * sends the message to server that has least amount of meetings.
 * The server then responds with a LISTOFMEETINGS_SERVER message which
 * contains all meetings on that server.
 */
int create_CREATENEWMEETING_CONTROLLER_message(char *buffer, int buffer_len,  char *topic);
/*
 * This function creates QUIT message that Client sends to Controller
 * when it wants to quit the connection.
 */
int create_QUIT_message(char *buffer, int buffer_len);
/*
 * This function creates GETDISCUSSIONS message that Client sends to 
 * Controller. The Controller responds with a message of known meetings
 */
int create_GETDISCUSSIONS_message(char *buffer, Meeting_server_list *list, int buffer_len);
/*
 * This function creates talk message that Client sends to 
 * Server. The Server sends the message all participants of that meeting.
 */
int create_TALK_message(char *message, Client_state *state, char *buffer, int buffer_len);



#endif
