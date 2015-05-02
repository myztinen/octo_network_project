#ifndef CLIENT_H
#define CLIENT_H
#include "connections.h"

struct client_state  {
    struct sockaddr_in connection;
    int my_socket;
    int connected;
    char *client_id;
    
};

typedef struct client_state Client_state; 

int parse_command(char *name, Client_state *state);



#endif
