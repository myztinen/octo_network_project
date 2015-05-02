#include "controller.h"


void sigint_handler(int sig)
{
   exit(EXIT_SUCCESS);
}

int read_meeting_list_message(char *message, Meeting_server_list *list, struct sockaddr_in *client) {
    char *temp_message, *next_pointer;
    char *id, *topic, *port, *amount;
    const char delimiter[2] = " ";
    Meeting_server *temp = list->head;

    temp_message = strdup(message);
    next_pointer = strtok(temp_message, delimiter);
    next_pointer = strtok(NULL, delimiter);
    while((strncmp(temp->server_id, next_pointer,10) != 0) && (temp != NULL)) {
        temp = temp->next;
    }
    if(temp == NULL) {
        perror("Did not find the correct meeting!!\n");
        return 1;
    }
    next_pointer = strtok(NULL, delimiter);
    //printf("Lukumäärä on %s\n", next_pointer);
    id = strtok(NULL, delimiter);
    while(id != NULL) { 
        
        //printf("Id on %s\n", id); 
        topic = strtok(NULL, delimiter);
        //printf("Topic on %s\n", topic);
        port = strtok(NULL, delimiter);
        //printf("Portti on %s\n", port);
        amount = strtok(NULL, delimiter);
        //printf("Osallistujat on %s\n", amount);
        Meeting *test_meeting = (void *) malloc(sizeof(Meeting));
        create_new_meeting(test_meeting, topic, id, client, amount);
        add_meeting_to_server(temp, test_meeting);
        id = strtok(NULL, delimiter);
    }
    free(temp_message);
    
}

int handle_server(int socket, char *send_buffer, char *rec_buffer,  struct sockaddr_in *server, Meeting_server_list *list) {
    struct sockaddr_in server_addr;
    int message_len;
    char *temp_message, *next_pointer;
    const char delimiter[2] = " ";
    
    while(1) {
        temp_message = strdup(rec_buffer);
        next_pointer = strtok(temp_message, delimiter);

        if(strncmp(IAMSERVER, next_pointer, strlen(IAMSERVER)) == 0) {
            printf("IAMSERVER vastaanotettu. Initialiosoidaan yhteys\n");

            next_pointer = strtok(NULL, delimiter);
            Meeting_server *new_server = (void *) malloc(sizeof(Meeting_server));
            
            create_new_server(new_server, next_pointer, server, socket);
            add_server_to_server_list(new_server, list);
            safe_write(socket, ACK, sizeof(ACK));
            print_meetings(list);
            printf("Yhteys initialisoitu\n");

        } else if(strncmp(MEETINGINFORMATION, next_pointer, strlen(MEETINGINFORMATION)) == 0) {

        } else if(strncmp(LISTOFMEETINGS_SERVER, next_pointer, strlen(LISTOFMEETINGS_SERVER)) == 0) {
            printf("LISTOFMEETINGS_SERVER received\n");
            read_meeting_list_message(rec_buffer, list, server);
            print_meetings(list);
        } else if(strncmp(MEETINGTERMINATED, next_pointer, strlen(MEETINGTERMINATED)) == 0) {
            printf("MEETINGTERMINATED received\n");
        } else if(strncmp(IAMCLIENT, next_pointer, strlen(IAMCLIENT)) == 0) {
            printf("IAMCLIENT vastaanotettu. Initialiosoidaan yhteys\n");
            safe_write(socket, ACK, sizeof(ACK));
        } else if(strncmp(GETDISCUSSIONS, next_pointer, strlen(GETDISCUSSIONS)) == 0) {
            printf("GETDISCUSSIONS vastaanotettu\n");
        } else if(strncmp(CREATENEWMEETING_CLIENT, next_pointer, strlen(CREATENEWMEETING_CLIENT)) == 0) {
            printf("CREATENEWMEETING_CLIENT received\n");
            next_pointer = strtok(NULL, delimiter);
            printf("Haluan luoda keskustelun nimeltä %s\n", next_pointer);
            message_len = create_CREATENEWMEETING_CONTROLLER_message(send_buffer, BUFFER_LENGTH, next_pointer);
            safe_write(list->head->socket, send_buffer, message_len);
            
        } else if(strncmp(QUIT, next_pointer, strlen(QUIT)) == 0) {
            close(socket);
            printf("QUIT received\n");
            pthread_exit(EXIT_SUCCESS);
            break;
        } else {
            printf("Did not understand the command\n");
        }
        safe_read(socket, rec_buffer, BUFFER_LENGTH);

    }
    
    free(temp_message);
    return 0;
}


int handle_client(int socket,  char *send_buffer, char *rec_buffer,  struct sockaddr_in *client)  {
    printf("Tulin client handleriin\n");
    char *temp_message, *next_pointer;
    const char delimiter[2] = " ";

    while(1) {
        temp_message = strdup(rec_buffer);
        memset((void *) rec_buffer, 0, BUFFER_LENGTH);
        next_pointer = strtok(temp_message, delimiter);

        if(strncmp(IAMCLIENT, next_pointer, strlen(IAMCLIENT)) == 0) {
        printf("IAMCLIENT vastaanotettu. Initialiosoidaan yhteys\n");
            safe_write(socket, ACK, sizeof(ACK));
        }
        else if(strncmp(GETDISCUSSIONS, next_pointer, strlen(GETDISCUSSIONS)) == 0) {
            printf("GETDISCUSSIONS vastaanotettu\n");
        } 
        else if(strncmp(CREATENEWMEETING_CLIENT, next_pointer, strlen(CREATENEWMEETING_CLIENT)) == 0) {
            printf("CREATENEWMEETING_CLIENT received\n");
        }
        else if(strncmp(QUIT, next_pointer, strlen(QUIT)) == 0) {
            close(socket);
            printf("QUIT received\n");
            pthread_exit(EXIT_SUCCESS);
            break;
        }
        else {
            printf("Did not understand the command\n");
        }
        safe_read(socket, rec_buffer, BUFFER_LENGTH);
    }
    printf("Lähden client handlerista\n");
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
    if((strncmp(rec_buffer, IAMSERVER, 2) == 0) || (strncmp(rec_buffer, IAMCLIENT, 2) == 0)) {
        handle_server(connfd, send_buffer, rec_buffer, &client_addr, list_pointer);
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
    struct sigaction sa;
    Thread_params *params;
    Meeting_server_list *the_list = (void *) malloc(sizeof(Meeting_server_list));
    init_meeting_server_list(the_list);
    
    
    sa.sa_handler = sigint_handler;
    sa.sa_flags = 0; // or SA_RESTART
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }       


    if (argc == 2)
        listenfd = tcp_listen(atoi(argv[1]), &controller_addr);
    else
        perror("usage: ./controller <port>\n");

    for ( ; ; ) {
        sockaddr_size =  sizeof(controller_addr);
        params = (Thread_params *) malloc(sizeof(Thread_params));
        params->meeting_server_list = the_list;
        params->socket = accept(listenfd, (struct sockaddr *) &(params->client), &sockaddr_size);


        pthread_create(&tid, NULL, &thread_handler, params);
    }
}


