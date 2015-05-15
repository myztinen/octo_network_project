#ifndef PARTICIPANT_H
#define PARTICIPANT_H
#include "project_defs.h"


/*
 * Adds new meeting participant to the participant list
 */
int add_participant_to_participant_list(int participant_socket, char *participant_id, Participant_list *list);

/*
 * Removes the participant from the participant list
 */
int remove_participant_from_participant_list(int participant_socket, Participant_list *list);

/*
 * Prints the current participant list. Used for debug purposes
 */
void print_participants(Participant_list *list);

#endif
