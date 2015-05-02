#include "client.h"

int parse_command(char *message,  Client_state *state) {
    char *temp_message, *next_pointer;
    const char delimiter[2] = " ";
    char send_buffer[BUFFER_LENGTH];
    int write_len=0;

    temp_message = strdup(message);
    next_pointer = strtok(temp_message, delimiter);

    if(strncmp("quitquit", next_pointer, strlen("quitquit")) == 0) {
        printf("quitquit Called\n");
        create_QUIT_message(send_buffer, BUFFER_LENGTH);
        safe_write(state->my_socket, send_buffer, BUFFER_LENGTH);
    }
    else if(strncmp("connect", next_pointer, strlen("connect")) == 0) {
        char *ip = strtok(NULL, delimiter);
        char *port = strtok(NULL, delimiter);
        state->my_socket = connect_to(ip, atoi(port), &(state->connection));
        create_IAMCLIENT_message(send_buffer, BUFFER_LENGTH, "Aapeli");
        safe_write(state->my_socket, send_buffer, BUFFER_LENGTH);
        printf("connect Called\n");
        free(temp_message);
        return state->my_socket;
    } 
    else if(strncmp("disconnect", next_pointer, strlen("disconnect")) == 0) {
        printf("disconnect Called\n");
    }
    else if(strncmp("getMeetings", next_pointer, strlen("getMeetings")) == 0) {
        printf("getMeetings Called\n");
    } 
    else if(strncmp("createMeeting", next_pointer, strlen("createMeeting")) == 0) {
        printf("createMeeting Called\n");
        next_pointer = strtok(NULL, delimiter);
        write_len = create_CREATENEWMEETING_CLIENT_message(send_buffer, BUFFER_LENGTH,  next_pointer);
        safe_write(state->my_socket, send_buffer, write_len);

    }
    else if(strncmp("talk", next_pointer, strlen("talk")) == 0) {
        printf("talk Called\n");
        next_pointer = strtok(NULL, delimiter);
        safe_write(state->my_socket, next_pointer, strlen(next_pointer));

    }
    else if(strncmp("talkTo", next_pointer, strlen("talkTo")) == 0) {
        printf("talkTo Called\n");
    }
    else if(strncmp("help", next_pointer, strlen("help")) == 0) {
        printf("help Called\n");
    }
    else if(strncmp("exit", next_pointer, strlen("help")) == 0) {
        printf("exit Called\n");
        exit(EXIT_SUCCESS);
    } 
    else {
        printf("Shit happened\n");
    }
    
    free(temp_message);
    return 0;

}

void echo_line_to_STD(char *name) {
    char buffer[BUFFER_LENGTH];

    int read_amount = 0, max_select, ready;
    Client_state my_state;
    memset((void *) &(my_state.connection), 0, sizeof(struct sockaddr_in));
    my_state.connected = 0;
    my_state.my_socket = 0;
    my_state.client_id = name;
    
    fd_set master, rset;
    max_select = STDIN_FILENO;
    memset(buffer, 0, BUFFER_LENGTH);
    FD_ZERO(&rset);
    FD_ZERO(&master);

    FD_SET(STDIN_FILENO, &master);

    while(1) {
        rset = master;
        printf("Tulin Selectiin\n");
        if((ready = select(max_select+1, &rset, NULL,NULL, NULL)) < 0) {
            if(errno == EINTR) continue;
            else perror("Error in select");
        }
        printf("Lähdin Selectistä\n");

        if(FD_ISSET(my_state.my_socket, &rset)) { 
            read_amount = recv(my_state.my_socket, buffer, BUFFER_LENGTH, 0);
            if(read_amount == 0) {
                    printf("Socket closed\n");
                        close(my_state.my_socket);
                        FD_CLR(my_state.my_socket, &master);
                        my_state.connected = 0;
                        my_state.my_socket = 0;
                    }
 
            write(STDOUT_FILENO, buffer, read_amount);
           
            memset(buffer, 0, BUFFER_LENGTH);

            }
        if(FD_ISSET(STDIN_FILENO, &rset)) { 
            int read_amount = 0, sent_amount = 0;

            if((read_amount = read(STDIN_FILENO, buffer, (ssize_t)BUFFER_LENGTH)) < 0)
                perror("Error in writing \n");
                
            if(strncmp(buffer, "\n", 1) == 0)
                continue;

            parse_command(buffer, &my_state);
             
            printf("SD on %d\n", my_state.my_socket);
            printf("Status on %d\n",  my_state.connected);

            if ( my_state.connected == 0 && my_state.my_socket != 0) {
                FD_SET(my_state.my_socket, &master);
                if (my_state.my_socket > max_select) {
                    max_select = my_state.my_socket;
                }
                 my_state.connected  = 1;
            }
            if((sent_amount = write(STDOUT_FILENO, buffer, read_amount)) < 0)
                perror("Error in writing\n");
            write(STDOUT_FILENO, "\n", 1);

        }
    }
}


int main(int argc, char *argv[]) {

    
    if(argc != 2) {
        printf("Invalid arg count. Start by ./client <myName>\n");
        exit(EXIT_FAILURE);
    }

    echo_line_to_STD(argv[1]);

    exit(EXIT_SUCCESS);
    
}

