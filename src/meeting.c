#include "meeting.h"

void * init_meeting_server_list(Meeting_server_list *pointer) {
    pointer->list_mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
    pointer->amount = 0;
    pointer->head = NULL;
    return pointer;
}

Meeting* create_new_meeting(Meeting *new_meeting, char *new_topic, char *new_id, int new_participant_amount, int port) {
    memset((void *) new_meeting, 0, sizeof(Meeting));
    new_meeting->next = NULL;
    strncpy(new_meeting->meeting_topic, new_topic, 20);
    strncpy(new_meeting->meeting_id, new_id, 10);
    new_meeting->participant_amount = new_participant_amount;
    new_meeting->port = port;
    return new_meeting;
    
}

Meeting_server* create_new_server(Meeting_server *server, char *server_id, struct sockaddr_in *addr, int connection_socket) {
    memset((void *) server, 0, sizeof(Meeting_server));
    server->next = NULL;
    server->head = NULL;
    strncpy(server->server_id, server_id, 10);
    memcpy(&(server->server_address), addr, sizeof(struct sockaddr_in));
    server->amount_of_meetings = 0;
    server->socket = connection_socket;
    return server;
    
}

int add_meeting_to_server(Meeting_server *server, Meeting *new_meeting) {

    Meeting *temp_pointer = server->head;
    if(server->head == NULL) {
        server->head = new_meeting;
        server->amount_of_meetings +=1;
        return 1;
    } else {
        while(1) {
            if(temp_pointer->next == NULL) {
               temp_pointer->next = new_meeting;
               server->amount_of_meetings +=1;
                return 1;
            } else {
                temp_pointer = temp_pointer->next;
            }
        }
    }
}

int add_server_to_server_list(Meeting_server *server, Meeting_server_list *list) {
    Meeting_server *temp_pointer = list->head;
    if(list->head == NULL) {
        list->head = server;
        list->amount +=1;
        return 1;
    } else {
        while(1) {
            if(temp_pointer->next == NULL) {
               temp_pointer->next = server;
               list->amount +=1;
                return 1;
            } else {
                temp_pointer = temp_pointer->next;
            }
        }
    }
}


int remove_meeting_from_list(Meeting_server *server, char *removable_id) {
    Meeting *temp_pointer = server->head;
    Meeting *previous = server->head;
    while(temp_pointer != NULL) {
        if(strncmp(temp_pointer->meeting_id, removable_id, 10) == 0) {
            if(temp_pointer == server->head && server->amount_of_meetings==1) {
                server->head = NULL;
                server->amount_of_meetings -=1;
                free(temp_pointer);
                return 1;
            }
            if(temp_pointer->next == NULL) {
                previous->next = NULL;
                server->amount_of_meetings -=1;
                free(temp_pointer);
                return 1;
            } else {
                previous->next = temp_pointer->next;
                server->amount_of_meetings -=1;
                free(temp_pointer);
                return 1;
            }
        }
        previous = temp_pointer;
        temp_pointer = temp_pointer->next;
    }
    return 0;
}

void print_meetings(Meeting_server_list *server_list) {
    Meeting_server *temp_pointer = server_list->head;
    while(temp_pointer != NULL) {
        Meeting *temp_meeting = temp_pointer->head;
        while(temp_meeting != NULL) {
            printf("Id: %s Topic: %s\n", temp_meeting->meeting_id, temp_meeting->meeting_topic);
            temp_meeting = temp_meeting->next;
        }
        temp_pointer = temp_pointer->next;
    }
}
