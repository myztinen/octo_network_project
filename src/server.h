#include "connections.h"
#include "controller.h"


struct meeting_params  {
    Meeting_server *my_server;
    char *topic;
    int port;  
};
void handle_meeting(int socket);

typedef struct meeting_params Meeting_params; 

