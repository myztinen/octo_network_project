#include "participant.h"

int add_participant_to_participant_list(int participant_socket, char *participant_id, Participant_list *list) {
    Meeting_participant * new_participant = (void *) malloc(sizeof(Meeting_participant));
    if(strlen(participant_id) <=10 && strlen(participant_id) >1) {
        strncpy(new_participant->participant_id, participant_id, strlen(participant_id));
    } else {
        printf("The participant ID is illegal length\n");
        free(new_participant);
        return 0;    
    }
    new_participant->participant_socket = participant_socket;
    new_participant->next = NULL;
    Meeting_participant *temp_pointer = list->head;
    if(list->head == NULL) {
        list->head = new_participant;
        list->participant_amount +=1;
        return 1;
    } else {
        while(1) {
            if(temp_pointer->next == NULL) {
               temp_pointer->next = new_participant;
               list->participant_amount +=1;
                return 1;
            } else {
                temp_pointer = temp_pointer->next;
            }
        }
    }
}

int remove_participant_from_participant_list(int participant_socket, Participant_list *list) {
    Meeting_participant *temp_pointer, *previous;
    temp_pointer = list->head;

    while(temp_pointer != NULL) {
        if(temp_pointer->participant_socket == participant_socket) {
            if(temp_pointer == list->head) {
                list->head = temp_pointer->next;
                free(temp_pointer);
                return 1;
            } else {
                previous->next = temp_pointer->next;
                free(temp_pointer);
                return 1;
            }
        } else {
            previous = temp_pointer;
            temp_pointer = temp_pointer->next;
        }
    }
    return 0;
}

void print_participants(Participant_list *list) {
    Meeting_participant *temp_pointer = list->head;
    while(temp_pointer != NULL) {
        printf("Id: %s\n", temp_pointer->participant_id);
        temp_pointer = temp_pointer->next;
    }
}
