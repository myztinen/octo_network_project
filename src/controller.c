#include "connections.h"
#include "controller.h"


void sigint_handler(int sig)
{
   exit(EXIT_SUCCESS);
}

int handle_server(int socket, char *buffer) {
    printf("Tulin server handleriin\n");
    write(socket, ACK, 2);
    printf("Lähden server handlerista\n");
    return 0;
}


int handle_client(int socket, char *buffer) {
    printf("Tulin client handleriin\n");
    write(socket, NACK, 2);
    printf("Lähden client handlerista\n");
    return 0;
}

static void *thread_handler(void *params) {
    char rec_buffer[300];
    int connfd, read_amount;
    //connfd = *((int *) arg);
    connfd = ((Thread_params*) params)->socket;
    struct sockaddr_in client_addr = ((Thread_params*) params)->client;
    free(params);

    pthread_detach(pthread_self());

    read_amount = read(connfd, rec_buffer, 300);
    write(STDOUT_FILENO, rec_buffer, read_amount);
    if(strncmp(rec_buffer, IAMSERVER, 2)) {
        handle_server(connfd,rec_buffer);
    } else if(strncmp(rec_buffer, IAMCLIENT, 2)) {
        printf("Going here\n");

        handle_client(connfd, rec_buffer);
    } else {
        printf("Received something that I don't recognise\n");
        write(connfd, NACK, 2);
    }

    close(connfd);
       
    return(NULL);
}

int main(int argc, char **argv) {
    int listenfd, *iptr;
    pthread_t tid;
    socklen_t addrlen, sockaddr_size;
    struct sockaddr_in *cli_addr;
    struct sockaddr_in controller_addr;
    struct sigaction sa;
    Thread_params *params;
    
    
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
        iptr = malloc(sizeof(int));
        params = (Thread_params *) malloc(sizeof(Thread_params));
        params->socket = accept(listenfd, (struct sockaddr *) &(params->client), &sockaddr_size);


        pthread_create(&tid, NULL, &thread_handler, params);
    }
}


