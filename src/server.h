#include "connections.h"
#include "controller.h"
#include "participant.h"

/*
 * Parameters passed to the new meeting thread including the pointer to
 * the current server, topic of the new meeting and the port number of 
 * the meeting.
 */
typedef struct meeting_params  {
    Meeting_server *my_server;
    char *topic;
    int port;  
} Meeting_params;

/*
 * Parameters that are passed to the function when the timer expires.
 */
typedef struct timer_params {
        Meeting *this_meeting;
        Meeting_server *this_server;
        timer_t *timer;
    } Timer_params;

/*
 * Timer expiration handler function. The timer starts when new meeting
 * is created or its participant amount drops to 0; After 1 min the 
 * handler will stop the meeting gracefully and informs the Controller
 * that the meeting has bee stopped.
 */
static void *empty_meeting_timer(void  *params);

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
void meeting_runner(int socket, Participant_list *participants, Meeting *this_meeting, Meeting_server *this_server);


