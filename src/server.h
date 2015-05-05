#include "connections.h"
#include "controller.h"


struct meeting_params  {
    Meeting_server *my_server;
    char *topic;
    int port;  
};
typedef struct meeting_params Meeting_params; 

/*
 * Initialises the connection with Controller
 */
int init_controller_connection(int socket, char *send_buffer, char *rec_buffer, char *my_id);

/*
 * The function for new thread. Each new meeting is run byt its own 
 * thread. The meeting_handler function start listening the selected
 * port and communicates the parameters of the Server to the Controller.
 * After the new connections are ready, actual meeting is handled
 * in the meeting_runner function.
 */ 
static void *meeting_handler_thread(void *params);

/*
 * The actual meeting uses select system call to monitor several sockets
 * simultaneously. Once one socket sends data, we loop through all of
 * to see which one has data ready. THe recived data is the sent to
 * all other participants. 
 */
void meeting_runner(int socket);


