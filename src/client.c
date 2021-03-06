#include "client.h"


int parse_command(char *message,  Client_state *state) {
    char *temp_message, *next_pointer;
    const char delimiter[2] = " ";
    char send_buffer[BUFFER_LENGTH];
    int write_len=0;

    temp_message = strdup(message);
    next_pointer = strtok(temp_message, delimiter);

    if(strncmp("quitquit", next_pointer, strlen("quitquit")) == 0) {
        create_QUIT_message(send_buffer, BUFFER_LENGTH);
        safe_write(state->my_socket, send_buffer, BUFFER_LENGTH);
        shutdown(state->my_socket, SHUT_RD);

    } else if(strncmp("connect", next_pointer, strlen("connect")) == 0) {
        if(state->connected == 1) {
            printf("*******Already connected to somewhere! Disconnect first******\n");
            free(temp_message);
            return 0;
        }
        char *ip = strtok(NULL, delimiter);
        char *port = strtok(NULL, delimiter);
        state->my_socket = connect_to(ip, atoi(port), &(state->connection));
        if(state->my_socket == 0) {
            safe_write(STDIN_FILENO, "Connection was not succesfull!\n", 31);
            free(temp_message);
            return 0;
        }
        create_IAMCLIENT_message(send_buffer, BUFFER_LENGTH, state->client_id);
        safe_write(state->my_socket, send_buffer, BUFFER_LENGTH);
        state->connected = 1;
        free(temp_message);
        return state->my_socket;
    
    } else if(strncmp("getMeetings", next_pointer, strlen("getMeetings")) == 0) {
        safe_write(state->my_socket, GETDISCUSSIONS, 2);
    } else if(strncmp("list", next_pointer, strlen("list")) == 0) {
        safe_write(state->my_socket, "list", 5);
    } else if(strncmp("createMeeting", next_pointer, strlen("createMeeting")) == 0) {
        next_pointer = strtok(NULL, delimiter);
        if(strlen(next_pointer) > 20 ) {
            printf("*******Meeting topic must be less than 20 characters******\n");
            free(temp_message);
            return 0;
        }
        write_len = create_CREATENEWMEETING_CLIENT_message(send_buffer, BUFFER_LENGTH,  next_pointer);
        safe_write(state->my_socket, send_buffer, write_len);
    } else if(strncmp("talk", next_pointer, strlen("talk")) == 0) {
        next_pointer = strtok(NULL, delimiter);
        if(strlen(next_pointer) > 255 ) {
            printf("*******Your Message must be less than 255 characters******\n");
            free(temp_message);
            return 0;
        }
        write_len = create_TALK_message(next_pointer, state, send_buffer, BUFFER_LENGTH); 
        safe_write(state->my_socket, send_buffer, write_len);
    } else if(strncmp("help", next_pointer, strlen("help")) == 0) {
        printf("%s\n",HELPTEXT);
    } else if(strncmp("exit", next_pointer, strlen("help")) == 0) {
        printf("*************** Exiting pogram. GOODBYE ******************\n");
        exit(EXIT_SUCCESS);
    } else {
        printf("Did not understand the command. Try again\n");
    }
    
    free(temp_message);
    return 0;
}

void user_interface(char *name) {
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
    printf("********** Welcome to the Meeting client **********\n");
    printf("****** Type \"help\" to see available commands ******\n");

    while(1) {
        memset((void *)buffer, 0, BUFFER_LENGTH);
        rset = master;
        if((ready = select(max_select+1, &rset, NULL,NULL, NULL)) < 0) {
            if(errno == EINTR) continue;
            else perror("Error in select");
        }
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
            if((read_amount = read(STDIN_FILENO, buffer, (ssize_t)BUFFER_LENGTH)) < 0)
                perror("Error in writing \n");
                
            if(strncmp(buffer, "\n", 1) == 0)
                continue;

            parse_command(buffer, &my_state);

            if ( my_state.my_socket != 0) {
                FD_SET(my_state.my_socket, &master);
                if (my_state.my_socket > max_select) {
                    max_select = my_state.my_socket;
                }
                 my_state.connected  = 1;
            }
        }
    }
}


int main(int argc, char *argv[]) {
  
    if(argc != 2) {
        printf("Invalid arg count. Start by ./client <myName>\n");
        exit(EXIT_FAILURE);
    }
    if(strlen(argv[1]) > 10 || strlen(argv[1]) < 2) {
        printf("Your name must be between 2 and 10 chars\n");
        exit(EXIT_FAILURE);
    }
    user_interface(argv[1]);

    exit(EXIT_SUCCESS);
    
}

