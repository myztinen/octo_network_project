#include "controller.h"


void sig_handler_server(int sig) {
    printf("SIGPIPE HANDLED exiting\n");
    pthread_exit(EXIT_SUCCESS);
}

void sig_handler_client(int sig) {
    printf("SIGPIPE HANDLED exiting\n");
    pthread_exit(EXIT_SUCCESS);
}

void read_meeting_list_message(char *message, Meeting_server_list *list, struct sockaddr_in *client) {
    char *temp_message, *command_code, *server_id, *list_length;
    char *id, *topic, *amount;
    int port;
    const char delimiter[2] = " ";
    Meeting_server *temp = list->head;

    temp_message = strdup(message);
    command_code = strtok(temp_message, delimiter);
    server_id = strtok(NULL, delimiter);
    while((strncmp(temp->server_id, server_id, 10) != 0) && (temp != NULL)) {
        temp = temp->next;
    }
    if(temp == NULL) {
        perror("Did not find the correct meeting!!\n");
        return;
    }
    list_length = strtok(NULL, delimiter);
    printf("list length id on %s\n", list_length);
    id = strtok(NULL, delimiter);
    while(id != NULL) { 
        topic = strtok(NULL, delimiter);
        port = atoi(strtok(NULL, delimiter));
        amount = strtok(NULL, delimiter);
        Meeting *test_meeting = (void *) malloc(sizeof(Meeting));
        create_new_meeting(test_meeting, topic, id, amount, port);
        add_meeting_to_server(temp, test_meeting);
        id = strtok(NULL, delimiter);
    }
    free(temp_message);
    
}

int handle_server(int socket, char *send_buffer, char *rec_buffer,  struct sockaddr_in *server, Meeting_server_list *list) {
    int message_len;
    char *temp_message, *next_pointer;
    const char delimiter[2] = " ";
    Meeting_server *new_server;
    
    struct sigaction sa;
    sa.sa_handler = sig_handler_server;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGPIPE, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    } 
    
    while(1) {
        temp_message = strdup(rec_buffer);
        next_pointer = strtok(temp_message, delimiter);

        if(strncmp(IAMSERVER, next_pointer, strlen(IAMSERVER)) == 0) {
            printf("IAMSERVER Initialising connection\n");
            next_pointer = strtok(NULL, delimiter);
            new_server = (void *) malloc(sizeof(Meeting_server));
            create_new_server(new_server, next_pointer, server, socket);
            add_server_to_server_list(new_server, list);
            safe_write(socket, ACK, sizeof(ACK));
            printf("Connection initialized\n");
        } else if(strncmp(LISTOFMEETINGS_SERVER, next_pointer, strlen(LISTOFMEETINGS_SERVER)) == 0) {
            printf("LISTOFMEETINGS_SERVER received\n");
            read_meeting_list_message(rec_buffer, list, server);
            printf("ListOfMeetings updated\n");
        } else if(strncmp(MEETINGTERMINATED, next_pointer, strlen(MEETINGTERMINATED)) == 0) {
            printf("MEETING TERMINATED received\n");
            next_pointer = strtok(NULL, delimiter);
            
            remove_meeting_from_meeting_server_by_id(next_pointer, new_server);
            printf("Viesti on %s",rec_buffer);
            pthread_exit(EXIT_SUCCESS);
        } else {
            printf("Did not understand the command\n");
        }
        message_len = safe_read(socket, rec_buffer, BUFFER_LENGTH);
        if(message_len == 0) {
            printf("Socket closed\n");
            remove_server_from_meeting_server_list(new_server, list);
            close(socket);
            pthread_exit(EXIT_SUCCESS);
        }
    }
    
    free(temp_message);
    return 0;
}

int handle_client(int socket, char *send_buffer, char *rec_buffer,  struct sockaddr_in *server, Meeting_server_list *list) {
    int message_len;
    char *temp_message, *next_pointer;
    const char delimiter[2] = " ";
    
    struct sigaction sa;
    sa.sa_handler = sig_handler_client;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGPIPE, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }       
    while(1) {
        temp_message = strdup(rec_buffer);
        next_pointer = strtok(temp_message, delimiter);

        if(strncmp(IAMCLIENT, next_pointer, strlen(IAMCLIENT)) == 0) {
            printf("IAMCLIENT received.\n");
            safe_write(socket, "You have connected to controller\n", 33);
        } else if(strncmp(GETDISCUSSIONS, next_pointer, strlen(GETDISCUSSIONS)) == 0) {
            printf("GETDISCUSSIONS received\n");
            print_meetings(list);
            message_len = create_GETDISCUSSIONS_message(send_buffer, list, BUFFER_LENGTH);
            safe_write(socket, send_buffer, message_len);
            printf("Answer GETDISCUSSIONS sent\n");
        } else if(strncmp(CREATENEWMEETING_CLIENT, next_pointer, strlen(CREATENEWMEETING_CLIENT)) == 0) {
            printf("CREATENEWMEETING_CLIENT received\n");
            next_pointer = strtok(NULL, delimiter);
            printf("Haluan luoda keskustelun nimeltä %s\n", next_pointer);
            int selected_socket = select_server(list);
            message_len = create_CREATENEWMEETING_CONTROLLER_message(send_buffer, BUFFER_LENGTH, next_pointer);
            safe_write(selected_socket, send_buffer, message_len);
            
        } else if(strncmp(QUIT, next_pointer, strlen(QUIT)) == 0) {
            close(socket);
            printf("QUIT received\n");
            pthread_exit(EXIT_SUCCESS);
            break;
        } else {
            printf("Did not understand the command\n");
            safe_write(socket, "Did not understand command!\n", 28);

        }
        message_len = safe_read(socket, rec_buffer, BUFFER_LENGTH);
        if(message_len == 0) {
            printf("Socket closed\n");
            close(socket);
            pthread_exit(EXIT_SUCCESS);
        }

    }
    
    free(temp_message);
    return 0;
}

static void *thread_handler(void *params) {
    char send_buffer[BUFFER_LENGTH];
    char rec_buffer[BUFFER_LENGTH];

    int connfd;
    connfd = ((Thread_params*) params)->socket;
    struct sockaddr_in client_addr = ((Thread_params *) params)->client;
    Meeting_server_list *list_pointer =  ((Thread_params *) params)->meeting_server_list;
    free(params);


    pthread_detach(pthread_self());


    safe_read(connfd, rec_buffer, BUFFER_LENGTH);
    if(strncmp(rec_buffer, IAMSERVER, 2) == 0) {
        handle_server(connfd, send_buffer, rec_buffer, &client_addr, list_pointer);
    } else if(strncmp(rec_buffer, IAMCLIENT, 2) == 0) {
        handle_client(connfd, send_buffer, rec_buffer, &client_addr, list_pointer);
    } else {
        printf("Received something that I don't recognise\n");
        write(connfd, NACK, 2);
    }

    return(NULL);
}

int main(int argc, char **argv) {
    int listenfd;
    pthread_t tid;
    socklen_t sockaddr_size;
    struct sockaddr_in controller_addr;
    Thread_params *params;
    Meeting_server_list *the_list = (void *) malloc(sizeof(Meeting_server_list));
    init_meeting_server_list(the_list);
    
    if (argc == 2)
        listenfd = tcp_listen(atoi(argv[1]), &controller_addr);
    else
        perror("usage: ./controller <port>\n");

    while(1) {
        sockaddr_size =  sizeof(controller_addr);
        params = (Thread_params *) malloc(sizeof(Thread_params));
        params->meeting_server_list = the_list;
        params->socket = accept(listenfd, (struct sockaddr *) &(params->client), &sockaddr_size);
        
        if (params->socket == -1) {
            perror("Erro accepting new connection!!!");
            exit(EXIT_FAILURE);
        }


        pthread_create(&tid, NULL, &thread_handler, params);
    }
}


