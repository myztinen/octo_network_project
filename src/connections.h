#ifndef CONNECTIONS_H
#define CONNECTIONS_H

#include "project_defs.h"

int safe_write(int socket, const char *buffer, int len);
int safe_read(int socket, char *buffer, int len);

int connect_to(char *ip_address, int port, struct sockaddr_in *server_addr);

int tcp_listen(int port, struct sockaddr_in *serv_addr);

int create_HELLO_message(char *buffer, int buffer_len);
int create_IAMSERVER_message(char *buffer, int buffer_len, const char *server_id);
int create_IAMCLIENT_message(char *buffer, int buffer_len, const char *server_id);
int create_LISTOFMEETINGS_SERVER_message(char *buffer, int buffer_len, Meeting_server *list);
int create_LISTOFMEETINGS_CONTROLLER_message(char *buffer, int buffer_len, Meeting_server_list *list);
int create_CREATENEWMEETING_CLIENT_message(char *buffer, int buffer_len,  char *topic);
int create_CREATENEWMEETING_CONTROLLER_message(char *buffer, int buffer_len,  char *topic);
int create_QUIT_message(char *buffer, int buffer_len);


#endif
