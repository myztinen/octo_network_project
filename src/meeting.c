#include "meeting.h"

void init_meeting_server_list(Meeting_server_list *pointer) {
    pointer->list_mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
    pointer->amount = 0;
    pointer->head = NULL;
}

void create_new_meeting(Meeting *new_meeting, char *new_topic, char *new_id, int new_participant_amount, int port) {
    memset((void *) new_meeting, 0, sizeof(Meeting));
    new_meeting->next = NULL;
    strncpy(new_meeting->meeting_topic, new_topic, 20);
    strncpy(new_meeting->meeting_id, new_id, 10);
    new_meeting->participant_amount = new_participant_amount;
    new_meeting->port = port;
    
}

void create_new_server(Meeting_server *server, char *server_id, struct sockaddr_in *addr, int connection_socket) {
    memset((void *) server, 0, sizeof(Meeting_server));
    server->next = NULL;
    server->head = NULL;
    strncpy(server->server_id, server_id, 10);
    memcpy(&(server->server_address), addr, sizeof(struct sockaddr_in));
    server->amount_of_meetings = 0;
    server->socket = connection_socket;    
}

int add_meeting_to_server(Meeting_server *server, Meeting *new_meeting) {
    Meeting *temp_pointer = server->head;
    if(server->head == NULL) {
        server->head = new_meeting;
        server->amount_of_meetings +=1;
        return 1;
    } else {
        while(1) {
            if(strncmp(new_meeting->meeting_id, temp_pointer->meeting_id, 10) == 0) {
                return 0;
            } else if(temp_pointer->next == NULL) {
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
    pthread_mutex_lock(&(list->list_mutex));
    Meeting_server *temp_pointer = list->head;
    if(list->head == NULL) {
        list->head = server;
        list->amount +=1;
        pthread_mutex_unlock(&(list->list_mutex));
        return 1;
    } else {
        while(1) {
            if(temp_pointer->next == NULL) {
               temp_pointer->next = server;
               list->amount +=1;
                pthread_mutex_unlock(&(list->list_mutex));
                return 1;
            } else {
                temp_pointer = temp_pointer->next;
            }
        }
    }
}

int remove_server_from_meeting_server_list(Meeting_server *server, Meeting_server_list *list) {
    pthread_mutex_lock(&(list->list_mutex));
    Meeting_server *temp_pointer = list->head;
    Meeting_server *previous = list->head;
    Meeting *meeting_iter, *temp;
    while(temp_pointer->server_id != server->server_id) {
        previous = temp_pointer;
        temp_pointer = temp_pointer->next;
        
        if(temp_pointer != NULL) {
            printf("Server is not in the list\n");
            pthread_mutex_unlock(&(list->list_mutex));
            return 0;
        }
    }
    meeting_iter = temp_pointer->head;
    while(meeting_iter != NULL) {
        temp = meeting_iter->next;
        free(meeting_iter);
        meeting_iter = temp;
    }
    if(list->head == temp_pointer) {
        list->head =  temp_pointer->next;
        list->amount -=1;
        free(temp_pointer);
        pthread_mutex_unlock(&(list->list_mutex));
        return 1;
    }
    if(temp_pointer->next == NULL) {       
        previous->next = NULL;
        list->amount -=1;
        free(temp_pointer);
        pthread_mutex_unlock(&(list->list_mutex));
        return 1;
    } else {
        previous->next = temp_pointer->next;
        list->amount -=1;
        free(temp_pointer);
        pthread_mutex_unlock(&(list->list_mutex));
        return 1;
    }
    return 0;
}

int select_server(Meeting_server_list *list) {
    int socket = 0;
    int current_min = INT_MAX;
    Meeting_server *iter = list->head;
    while(iter != NULL) {
        if(iter->amount_of_meetings < current_min) {
            current_min = iter->amount_of_meetings;
            socket = iter->socket;
        }
        iter = iter->next;
    }
    return socket;
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
