#ifndef CONNECTIONS_H
#define CONNECTIONS_H

#include "project_defs.h"

#define BUFFER_LENGTH 300

int connect_to(char *ip_address, int port, struct sockaddr_in *server_addr);

int tcp_listen(int port, struct sockaddr_in *serv_addr);


#endif
