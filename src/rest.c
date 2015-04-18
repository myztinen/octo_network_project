int create_server(int server_port) {
        int listenfd, client_socket, read_amount, my_pid;
        pid_t actual_pid;
        socklen_t clilen;
        struct sockaddr_in cli_addr, serv_addr;


        if ((listenfd = socket(AF_INET, SOCK_STREAM, PF_UNSPEC)) < 0) {
            perror("Creating stream socket");
        exit(EXIT_FAILURE);
    }
    memset(&serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        serv_addr.sin_port = htons(server_port);
        if ((bind(listenfd, (struct sockaddr *) &serv_addr, sizeof (serv_addr))) < 0) {
            perror("Binding local address");
        exit(EXIT_FAILURE);
    }
        listen(listenfd, 5);

    for (;;) {

            clilen = sizeof(cli_addr);
            if((client_socket = accept(listenfd, (struct sockaddr *) &cli_addr, &clilen)) < 0) {
            perror("Could not accept incoming connection\n");
            exit(EXIT_FAILURE);
        }
        // Following lines are just to allow the user to prevent exess forking in case of bad coding    
        sleep(1);
        if((my_pid = fork()) < 0) {
            perror("Could not fork\n");
            exit(EXIT_FAILURE);
        } else if(my_pid == 0) {
            actual_pid = getpid();
            char server_read_buffer[BUFFER_LENGTH];
            close(listenfd);
                while ((read_amount = recv(client_socket, server_read_buffer, BUFFER_LENGTH, 0)) > 0) {
                    switch(parse_command_from_message(server_read_buffer)) {
                    case 'E' :
                        printf("Pid %d got E \n", actual_pid);
                        break;
                    case 'C' :
                        printf("Pid %d got C \n", actual_pid);
                        break;
                    case 'F' :
                        printf("Pid %d got F \n", actual_pid);
                        break;
                    case 'A' :
                        printf("Pid  %d got A \n", actual_pid);                       
                        break;
                    case 'Q' :
                        printf("Pid %d got Q \n", actual_pid);           
                        break;
                    case '#' :
                        printf("Pid %d got C \n", actual_pid);
                        break;
                    default :
                        printf("Pid %d got command that I didn't understand\n", actual_pid);
                        exit(EXIT_FAILURE);
                }//switch
            }//while
            close(client_socket);
        exit(EXIT_SUCCESS);
        } //else
    }//For loop
}

char parse_command_from_message(char *message_buffer) {
        return message_buffer[0];
}


int copy_partial_string(char *src_buffer, char *dst_buffer, int start, int len) {
    char * p;
    p = src_buffer+start;
    strncpy(dst_buffer, p, len);
    return 1;
}

void convert_to_lower_case(char *src_buffer, char *result_buffer, int length) {
        int i;

        for(i = 0; i <= length; i++) {
                result_buffer[i] = tolower(src_buffer[i]);
        }
}

int count_nth_whitespace(char *buffer, int n) {
    int i = 0;
    int index = 0, counter=0;
    while(i<strlen(buffer)) {
        if(buffer[i] == ' ') {
            index = i;
            counter++;
        }
        i++;
    if(counter==n) break;
    }
    return index;
}



int get_port_number(char *buffer) {
    char temp_buffer[6];
        int port_number = 0;
        int start = count_nth_whitespace(buffer, 2);
    printf("Start is %d \n", start);
        copy_partial_string(buffer, temp_buffer, (start+1), 5);
    temp_buffer[6]='\0';
    sscanf(temp_buffer, "%d", &port_number);
        return port_number;
}
