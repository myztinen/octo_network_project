#include "server.h"
void sigint_handler(int sig)
{
    write(0, "Ahhh! SIGINT!\n", 14);
    exit(EXIT_SUCCESS);
}

int init_controller_connection(int socket, char *send_buffer, char *rec_buffer, char *my_id) {
    int message_len;

    message_len = create_IAMSERVER_message(send_buffer, BUFFER_LENGTH, my_id);
    safe_write(socket, send_buffer, message_len);
    safe_read(socket, rec_buffer, BUFFER_LENGTH);
    if(strncmp(rec_buffer, ACK, 2) == 0) {
        printf("viesti on %s. Init suoritettu! \n", rec_buffer);
        return 1;
    } else {
        perror("Could not initialize connection to server\n");
        exit(EXIT_FAILURE);
    }
}



static void *meeting_handler_thread(void *params) {
    char send_buffer[BUFFER_LENGTH];
    int message_len;
    Meeting_server *server_pointer =  ((Meeting_params *) params)->my_server;
    char *new_topic = ((Meeting_params *) params)->topic;
    printf("Topic on %s \n", new_topic);
    int my_port = ((Meeting_params *) params)->port;
    free(params);
    Meeting *this_meeting = (void *) malloc(sizeof(Meeting));
    struct sockaddr_in meeting_address;
    int meeting_socket;
    char meeting_id[10];
    pthread_detach(pthread_self());

    meeting_socket = tcp_listen(my_port, &meeting_address);
    srand(time(NULL));
    snprintf(meeting_id, 10, "%d",  (rand()%10000));
    create_new_meeting(this_meeting, new_topic, meeting_id, 0, my_port);
    add_meeting_to_server(server_pointer, this_meeting);
    message_len = create_LISTOFMEETINGS_SERVER_message(send_buffer, BUFFER_LENGTH, server_pointer);
    safe_write(server_pointer->socket, send_buffer, message_len);
    handle_meeting(meeting_socket);
    return(NULL);
}

void handle_meeting(int socket) {
    fd_set master;   
    fd_set read_fds; 
    int fdmax;

    int client_fd; 
    struct sockaddr_storage remoteaddr;
    socklen_t addrlen;

    char buf[BUFFER_LENGTH];  
    int message_len;

    int iterable_fd, other_fd;


    FD_ZERO(&master);    
    FD_ZERO(&read_fds);

    FD_SET(socket, &master);
    fdmax = socket; 

    // main loop
    while(1) {
        printf("Meeting started.\n");
        read_fds = master; 
        if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("Error in select!!!!");
            exit(EXIT_FAILURE);
        }

        // Loop through all existing connections
        for(iterable_fd = 0; iterable_fd <= fdmax; iterable_fd++) {
            if (FD_ISSET(iterable_fd, &read_fds)) {
                // If the connection is new connction
                if (iterable_fd == socket) {
                    addrlen = sizeof remoteaddr;
                    client_fd = accept(socket,(struct sockaddr *)&remoteaddr,&addrlen);

                    if (client_fd == -1) {
                        perror("Erro accepting new connection!!!");
                        exit(EXIT_FAILURE);
                    } else {
                        FD_SET(client_fd, &master); 
                        if (client_fd > fdmax) {    
                            fdmax = client_fd;
                        }
                        printf("New connection from on socket %d\n",client_fd);
                    }
                // Read from existing connection
                } else {
                    // If read amount is 0, the client has closed a connection
                    if ((message_len = read(iterable_fd, buf, BUFFER_LENGTH)) <= 0) {
                        if (message_len == 0) {
                            printf("Socket number %d closed connection\n", iterable_fd);
                        } else {
                            perror("recv");
                        }
                        close(iterable_fd); 
                        FD_CLR(iterable_fd, &master);
                    } else {
                        for(other_fd = 0; other_fd <= fdmax; other_fd++) {
                            if (FD_ISSET(other_fd, &master)) {
                                if (other_fd != socket && other_fd != iterable_fd) {
                                    if (write(other_fd, buf, message_len) == -1) {
                                        perror("Error in sending");
                                    }
                                }
                            }
                        }
                    }
                }
            }
        } 
    }
    pthread_exit(EXIT_SUCCESS);
}


int main(int argc, char *argv[]) { 
    int controller_socket, read_amount, write_amount, message_length = 0, port = 50001;
    struct sockaddr_in controller_addr;
    pthread_t thread_id;
    struct sigaction sa;
    char read_buffer[BUFFER_LENGTH];
    char send_buffer[BUFFER_LENGTH];
    char *temp_message, *next_pointer;
    const char delimiter[2] = " ";


    
    if(argc != 4) {
        printf("Invalid arg count. Start by ./server <ip address> <port> <server id>\n");
        exit(EXIT_FAILURE);
    }

    Meeting_server *my_meetings = (void *)malloc(sizeof(Meeting_server));
    strcpy(my_meetings->server_id, argv[3]);
    my_meetings->amount_of_meetings = 0;
    my_meetings->head = NULL;
    my_meetings->next = NULL;
    my_meetings->socket = 0;
    

    sa.sa_handler = sigint_handler;
    sa.sa_flags = 0; 
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGPIPE, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }       

    my_meetings->socket = connect_to(argv[1], atoi(argv[2]), &controller_addr);
    init_controller_connection(my_meetings->socket, send_buffer, read_buffer, argv[3]);

    message_length = create_LISTOFMEETINGS_SERVER_message(send_buffer, BUFFER_LENGTH, my_meetings);
    write_amount =  safe_write(my_meetings->socket, send_buffer, message_length);    
    while(1) {
        read_amount = safe_read(my_meetings->socket,read_buffer, BUFFER_LENGTH);
        temp_message = strdup(read_buffer);
        next_pointer = strtok(temp_message, delimiter);

        if(strncmp(CREATENEWMEETING_CONTROLLER, next_pointer, strlen(CREATENEWMEETING_CONTROLLER)) == 0) {
            Meeting_params *myparams = (void *) malloc(sizeof(Meeting_params));
            next_pointer = strtok(NULL, delimiter);
            myparams->my_server = my_meetings;
            port +=1;
            myparams->port = port;
            myparams->topic = next_pointer;
            pthread_create(&thread_id, NULL, &meeting_handler_thread, myparams);
        } else {
            printf("Did not understand the command code : %s\n", next_pointer);
            safe_write(my_meetings->socket, NACK, 2);
        }
    }    
    close(controller_socket); 
        exit(EXIT_SUCCESS);
}
