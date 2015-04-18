#include "client.h"

#define BUFFER_LENGTH 300


void echo_line_to_STD(char *buffer, int buffer_size) {
    int read_amount = 0, max_select, ready, sd = 0;
    int status = 0;
    fd_set master, rset;
    max_select = STDIN_FILENO;
    memset(buffer, 0, buffer_size);
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

        if(FD_ISSET(sd, &rset)) { 
            read_amount = recv(sd, buffer, buffer_size, 0);
            if(read_amount == 0) {
                    printf("Socket closed\n");
                        close(sd);
                        FD_CLR(sd, &master);
                        status = 0;
                        sd = 0;
                    }
 
            write(STDOUT_FILENO, buffer, read_amount);
           
            memset(buffer, 0, buffer_size);

            }
        if(FD_ISSET(STDIN_FILENO, &rset)) { 
            int read_amount = 0, sent_amount = 0;

            if((read_amount = read(STDIN_FILENO, buffer, (ssize_t)buffer_size)) < 0)
                perror("Error in writing \n");
                
            if(strncmp(buffer, "\n", 1) == 0)
                continue;
            if(sd == 0) {
                sd = parse_command(buffer, 3);
            } else {
                parse_command(buffer, 3);
            }
            printf("SD on %d\n", sd);
            printf("Status on %d\n", status);

            if (status == 0 && sd != 0) {
                FD_SET(sd, &master);
                if (sd > max_select) {
                    max_select = sd;
                }
                status  = 1;
            }
            if((sent_amount = write(STDOUT_FILENO, buffer, read_amount)) < 0)
                perror("Error in writing\n");
            write(STDOUT_FILENO, "\n", 1);

        }
    }
}




int create_listening_socket(int port) {
    int listenfd;

    struct sockaddr_in serv_addr;

    if ( (listenfd = socket(AF_INET, SOCK_STREAM, PF_UNSPEC)) < 0) {
        perror("Error in creating stream socket");
        exit(EXIT_FAILURE);
    }
    
    memset((void *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);
    if ((bind(listenfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr))) < 0)
        perror("Binding local address") ;
    listen(listenfd, 5);
    
    return listenfd;
}


int main(int argc, char *argv[]) {
    int my_socket, server_listening_socket;
    pid_t my_pid;

    char my_buffer[BUFFER_LENGTH];

    /*
    if((my_socket = tcpConnect(argv[1], atoi(argv[2]))) == 0) {
        perror("Did not get the socket\n");
        exit(EXIT_FAILURE);
    }*/
    echo_line_to_STD(my_buffer, BUFFER_LENGTH);

    exit(EXIT_SUCCESS);
    
}

