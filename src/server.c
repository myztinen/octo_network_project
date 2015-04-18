#include "connections.h"

void sigint_handler(int sig)
{
    write(0, "Ahhh! SIGINT!\n", 14);
}



main(int argc, char *argv[]){ 
    int listenfd, controller_socket, clilen, read_amount, write_amount;
    struct sockaddr_in meeting_addr, controller_addr;
    struct sigaction sa;
    
    char read_buffer[300];
    
    sa.sa_handler = sigint_handler;
    sa.sa_flags = 0; // or SA_RESTART
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGPIPE, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }       

    controller_socket = connect_to(argv[1], atoi(argv[2]), &controller_addr);

    write_amount =  write(controller_socket, IAMSERVER, 2);
    printf("Kirjoitin %d tavua\n", write_amount);
    
    read_amount = read(controller_socket,read_buffer, 300);
    
    printf("Luin %d tavua ja viesti oli %s\n", read_amount, read_buffer);

    
    close(controller_socket); 
        exit(EXIT_SUCCESS);
}
