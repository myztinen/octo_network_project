#include "server.h"
void sigint_handler(int sig)
{
    write(0, "Ahhh! SIGINT!\n", 14);
    exit(EXIT_SUCCESS);
}

int init_controller_connection(int socket, char *send_buffer, char *rec_buffer, char *my_id) {
    int message_len, write_amount, read_amount;

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
    char rec_buffer[BUFFER_LENGTH];
    int message_len;
    Meeting_server *server_pointer =  ((Meeting_params *) params)->my_server;
    char *new_topic = ((Meeting_params *) params)->topic;
    printf("Topic on %s \n", new_topic);
    int my_port = ((Meeting_params *) params)->port;
    free(params);
    Meeting *this_meeting = (void *) malloc(sizeof(Meeting));
    int controller_socket = server_pointer->socket;
    struct sockaddr_in meeting_address, client_addr;
    int meeting_socket, client_socket;
    int socket_size = sizeof(struct sockaddr_in);
    char meeting_id[10];
    pthread_detach(pthread_self());

    meeting_socket = tcp_listen(my_port, &meeting_address);
    printf("Loin äsken uuden socketin meetingille\n");
    snprintf(meeting_id, 10, "%d",  (rand()%10000));

    create_new_meeting(this_meeting, new_topic, meeting_id, 0, my_port);
    add_meeting_to_server(server_pointer, this_meeting);
    message_len = create_LISTOFMEETINGS_SERVER_message(send_buffer, BUFFER_LENGTH, server_pointer);
    printf("Nyt kirjoitan tiedot controllerille\n");
    safe_write(server_pointer->socket, send_buffer, message_len);
    
    printf("kirjoitettu! Nyt enään luoen omaa sockettia\n");
    handle_meeting(meeting_socket);
    /*client_socket = accept(meeting_socket, (struct sockaddr *) &(client_addr), &socket_size);
    printf("Yhteys hyväksytty!!\n");

    safe_read(client_socket, rec_buffer, BUFFER_LENGTH);
    printf("Meeting luki juuri %s\n", rec_buffer);
    safe_write(client_socket, ACK, 2);*/
    return(NULL);
}

int handle_meeting(int socket) {
    fd_set master;    // master file descriptor list
    fd_set read_fds;  // temp file descriptor list for select()
    int fdmax;        // maximum file descriptor number

    int listener;     // listening socket descriptor
    int newfd;        // newly accept()ed socket descriptor
    struct sockaddr_storage remoteaddr; // client address
    socklen_t addrlen;

    char buf[256];    // buffer for client data
    int nbytes;

    int i, j, rv;


    FD_ZERO(&master);    // clear the master and temp sets
    FD_ZERO(&read_fds);

    // add the listener to the master set
    FD_SET(socket, &master);

    // keep track of the biggest file descriptor
    fdmax = socket; // so far, it's this one

    // main loop
    for(;;) {
        read_fds = master; // copy it
        if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(4);
        }

        // run through the existing connections looking for data to read
        for(i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) { // we got one!!
                if (i == socket) {
                    // handle new connections
                    addrlen = sizeof remoteaddr;
                    newfd = accept(socket,(struct sockaddr *)&remoteaddr,&addrlen);

                    if (newfd == -1) {
                        perror("accept");
                    } else {
                        FD_SET(newfd, &master); // add to master set
                        if (newfd > fdmax) {    // keep track of the max
                            fdmax = newfd;
                        }
                        printf("selectserver: new connection from on "
                            "socket %d\n",newfd);
                    }
                } else {
                    // handle data from a client
                    if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0) {
                        // got error or connection closed by client
                        if (nbytes == 0) {
                            // connection closed
                            printf("selectserver: socket %d hung up\n", i);
                        } else {
                            perror("recv");
                        }
                        close(i); // bye!
                        FD_CLR(i, &master); // remove from master set
                    } else {
                        // we got some data from a client
                        for(j = 0; j <= fdmax; j++) {
                            // send to everyone!
                            if (FD_ISSET(j, &master)) {
                                // except the listener and ourselves
                                if (j != socket && j != i) {
                                    if (send(j, buf, nbytes, 0) == -1) {
                                        perror("send");
                                    }
                                }
                            }
                        }
                    }
                } // END handle data from client
            } // END got new incoming connection
        } // END looping through file descriptors
    } // END for(;;)--and you thought it would never end!
    
    return 0;
}


main(int argc, char *argv[]) { 
    int listenfd, controller_socket, clilen, read_amount, write_amount, message_length = 0, port = 50001;
    struct sockaddr_in controller_addr;
    pthread_t thread_id;
    struct sockaddr_un local, remote;
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
            printf("Luon kohta threadin\n");
            Meeting_params *myparams = (void *) malloc(sizeof(Meeting_params));
            next_pointer = strtok(NULL, delimiter);
            myparams->my_server = my_meetings;
            myparams->port += port;
            myparams->topic = next_pointer;
            pthread_create(&thread_id, NULL, &meeting_handler_thread, myparams);
        } else if(strncmp(LISTOFMEETINGS_CONTROLLER, next_pointer, strlen(LISTOFMEETINGS_CONTROLLER)) == 0) {


        } else {
            printf("Did not understand the command code : %s\n", next_pointer);
            safe_write(my_meetings->socket, NACK, 2);
        }
    }
    printf("Luin %d tavua ja viesti oli %s\n", read_amount, read_buffer);
    printf("Kirjoitin %d tavua\n", write_amount);
    
    
    close(controller_socket); 
        exit(EXIT_SUCCESS);
}
