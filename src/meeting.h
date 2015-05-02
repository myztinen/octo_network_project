#ifndef MEETING_H
#define MEETING_H
#include "project_defs.h"


void * init_meeting_server_list(Meeting_server_list *pointer);
Meeting* create_new_meeting(Meeting *new_meeting, char *new_topic, char *new_id, int new_participant_amount, int port);
Meeting_server* create_new_server(Meeting_server *server, char *server_id, struct sockaddr_in *addr, int connection_socket);
int add_server_to_server_list(Meeting_server *server, Meeting_server_list *list);
int add_meeting_to_list(Meeting_server_list *list, Meeting *new_meeting);
int remove_meeting_from_list(Meeting_server *server, char *removable_id);
void print_meetings(Meeting_server_list *server_list);


#endif
