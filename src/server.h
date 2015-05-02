#include "connections.h"
#include "controller.h"


struct meeting_params  {
    Meeting_server *my_server;
    char *topic;
    int port;  
};

int handle_meeting(int meeting_socket);
typedef struct meeting_params Meeting_params; 

