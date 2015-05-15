#ifndef MEETING_H
#define MEETING_H
#include "project_defs.h"

/*
 * Inititialize Meeting_server_list with initial values
 */
void init_meeting_server_list(Meeting_server_list *pointer);
/*
 * Inititialize Meeting with given values.
 */
void create_new_meeting(Meeting *new_meeting, char *new_topic, char *new_id, int new_participant_amount, int port);
/*
 * Inititialize Meeting_server with given values.
 */
void create_new_server(Meeting_server *server, char *server_id, struct sockaddr_in *addr, int connection_socket);

/*
 * Remove given meeting from Meeting server data structure by given pointer.
 */
int remove_meeting_from_meeting_server(Meeting *this_meeting, Meeting_server *list);

/*
 * Remove given meeting from Meeting server data structure by given meeting id
 */
int remove_meeting_from_meeting_server_by_id(char *meeting_id, Meeting_server *server);

/*
 * Adds the information from connected server to the Meeting_server_list
 */
int add_server_to_server_list(Meeting_server *server, Meeting_server_list *list);
/*
 * Adds the information from Meeting to the Meeting_server
 */
int add_meeting_to_list(Meeting_server_list *list, Meeting *new_meeting);
/*
 * Removes the Server and the meetings that it contains from 
 * Meeting_server_list.
 */
int remove_server_from_meeting_server_list(Meeting_server *server, Meeting_server_list *list);
/*
 * Prints all meetings known to Controller. Used for debugging purposes
 */
void print_meetings(Meeting_server_list *server_list);
/*
 * Controller uses this to select Server application that has the least
 * amount of meetings.
 */
int select_server(Meeting_server_list *list);



#endif
