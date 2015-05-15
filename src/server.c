#include "server.h"

void sigint_handler(int sig)
{
    write(STDOUT_FILENO, "SIGPIPE received!\n", 18);
    pthread_exit(NULL);
}



static void *empty_meeting_timer(void  *params) {   
    char buffer[30];
    int message_len = 0;
    
    message_len = create_MEETINGTERMINATED_message(buffer, ((Timer_params *)params)->this_meeting, 30);
    safe_write(((Timer_params *)params)->this_server->socket, buffer, message_len);
    if(timer_delete(*(((Timer_params *)params)->timer)) < 0)
        perror("Could not delete timer!!\n");
    printf("Removed meeting with topic %s\n", ((Timer_params *)params)->this_meeting->meeting_topic);
    remove_meeting_from_meeting_server(((Timer_params *)params)->this_meeting, ((Timer_params *)params)->this_server);
    pthread_exit(NULL);

}


int init_controller_connection(int socket, char *send_buffer, char *rec_buffer, char *my_id) {
    int message_len;
    message_len = create_IAMSERVER_message(send_buffer, BUFFER_LENGTH, my_id);
    safe_write(socket, send_buffer, message_len);
    safe_read(socket, rec_buffer, BUFFER_LENGTH);
    if(strncmp(rec_buffer, ACK, 2) == 0) {
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
    int my_port = ((Meeting_params *) params)->port;
    free(params);
    Meeting *this_meeting = (void *) malloc(sizeof(Meeting));
    struct sockaddr_in meeting_address;
    int meeting_socket;
    char meeting_id[10];
    pthread_detach(pthread_self());
    
    /* Start listening the socket for meeting attendees and add the 
     * Meeting data structure to the common Meeting_server data structure.
     */
    meeting_socket = tcp_listen(my_port, &meeting_address);
    srand(time(NULL));
    snprintf(meeting_id, 10, "%d",  (rand()%10000));
    create_new_meeting(this_meeting, new_topic, meeting_id, 0, my_port);
    add_meeting_to_server(server_pointer, this_meeting);
    /*
     * Create participant list for this meeting
     */
    Participant_list *attendee_list = (void *) malloc(sizeof(Participant_list));
    attendee_list->participant_amount = 0;
    attendee_list->head = NULL;
    
    /*
     * Relay the current meetings from this server to the Controller and
     * start the actual meeting.
     */
    message_len = create_LISTOFMEETINGS_SERVER_message(send_buffer, BUFFER_LENGTH, server_pointer);
    safe_write(server_pointer->socket, send_buffer, message_len);
    meeting_runner(meeting_socket, attendee_list, this_meeting, server_pointer);
}

void create_idle_timer(timer_t *timer,  void *params ) {
        struct sigevent *sev = (void *) malloc(sizeof(struct sigevent));
        sev->sigev_notify = SIGEV_THREAD;
        sev->sigev_notify_function = (void *) empty_meeting_timer; 
        sev->sigev_value.sival_ptr = (void *) params;
        
        if(timer_create(CLOCK_REALTIME, sev, timer) < 0) {
            perror("Error creating a timer. Exting...\n");
            pthread_exit(NULL);
        }

}

void start_timer(timer_t *timer, int waiting_time, struct itimerspec *its ) {        
        its->it_value.tv_sec = waiting_time;
        its->it_value.tv_nsec = 0;
        its->it_interval.tv_sec = its->it_value.tv_sec;
        its->it_interval.tv_nsec = its->it_value.tv_nsec;
        if(timer_settime(*timer, 0, its, NULL) < 0) {
            perror("Error starting the timer. Exting...\n");
            pthread_exit(NULL);
        }
        printf("No participants. Meeting will be removed after %d seconds\n", waiting_time);
}

void stop_timer(timer_t *timer, struct itimerspec *its ) {
        
        its->it_value.tv_sec = 0;
        its->it_value.tv_nsec = 0;
        its->it_interval.tv_sec = its->it_value.tv_sec;
        its->it_interval.tv_nsec = its->it_value.tv_nsec;
        if(timer_settime(*timer, 0, its, NULL) < 0) {
            perror("Error stopping the timer. Exting...\n");
            pthread_exit(NULL);
        }
        printf("Removal timer stopped\n");
}


int update_participants(int socket_fd, char *message_buffer, Participant_list *list) {
    const char delimiter[2] = " ";
    char *next_pointer = strtok(message_buffer, delimiter);
    char *name = strtok(NULL, delimiter);
    return add_participant_to_participant_list(socket_fd, name, list);                    
}

void meeting_runner(int socket, Participant_list *participants, Meeting *this_meeting, Meeting_server *this_server) {
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
    
    //Timer
    struct itimerspec its;
    timer_t my_timer;
    Timer_params *params = (void *) malloc(sizeof(Timer_params));
    params->this_meeting = this_meeting;
    params->this_server = this_server;
    params->timer = &my_timer;
    
    create_idle_timer( &my_timer,  (void *) params );
    start_timer(&my_timer, 60, &its);
    printf("Meeting started.\n");

    // main loop
    while(1) {
        read_fds = master; 
        if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("Error in select!!!!");
            exit(EXIT_FAILURE);
        }

        // Loop through all existing connections
        for(iterable_fd = 0; iterable_fd <= fdmax; iterable_fd++) {
            if (FD_ISSET(iterable_fd, &read_fds)) {
                // If the connection is new connction, accept connection
                // and add it to master fd set-
                if (iterable_fd == socket) {
                    addrlen = sizeof remoteaddr;
                    client_fd = accept(socket,(struct sockaddr *)&remoteaddr,&addrlen);
                    safe_read(client_fd, buf, BUFFER_LENGTH);
                    update_participants(client_fd, buf, participants);
                    this_meeting->participant_amount += 1;
                    if(this_meeting->participant_amount == 1)
                        stop_timer(&my_timer, &its);
                    
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
                        remove_participant_from_participant_list(iterable_fd, participants);
                        this_meeting->participant_amount -= 1;
                        close(iterable_fd); 
                        FD_CLR(iterable_fd, &master);
                        if(this_meeting->participant_amount == 0)
                                start_timer(&my_timer, 60, &its);
                        /* If message is from existing connection, send
                         * them to every socket exept our Meetings
                         * socket and senders socket. Also request list
                         * of current participants.
                         */    
                    } else {
                        // If client requested for participant list
                        if(strncmp(buf, "list", 5) == 0) {
                            message_len = create_participant_list_message(buf, participants, BUFFER_LENGTH);
                            safe_write(iterable_fd, buf, message_len);
                        // Send normal message to all participants
                        } else {
                            for(other_fd = 0; other_fd <= fdmax; other_fd++) {
                                if (FD_ISSET(other_fd, &master)) {
                                    if (other_fd != socket && other_fd != iterable_fd) {
                                        if (write(other_fd, buf, message_len) == -1) {
                                            perror("Error in sending the message\n");
                                        }
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
    
    if(strlen(argv[3]) != 10)  {
        printf("Server Id must be 10 chars long\n");
        exit(EXIT_FAILURE);
    }
    
    //Initialise the Meeting_server parameters
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
        if(read_amount == 0) {
            printf("Socket closed\n");
            close(my_meetings->socket);
            exit(EXIT_FAILURE);
        }
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
