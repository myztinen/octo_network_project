#include "connections.h"


int connect_to(char *ip_address, int port, struct sockaddr_in *server_addr) {
    int return_value, handle;
    

    if ((handle = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror ("Could not create socket\n");
        handle = 0;
    } else {
        server_addr->sin_family = AF_INET;
        server_addr->sin_port = htons(port);
        inet_aton(ip_address, &(server_addr->sin_addr));

        if((return_value = connect(handle, (struct sockaddr*)  server_addr, sizeof(struct sockaddr))) < 0) {
            perror("Could not connect\n");
            handle = 0;
        }
    }
    return handle;
}

int tcp_listen(int port, struct sockaddr_in *serv_addr) {
    int listening_socket;
    if ( (listening_socket = socket(AF_INET, SOCK_STREAM, PF_UNSPEC)) < 0) {
        perror("Creating stream socket");
        exit(EXIT_FAILURE);
    }

    serv_addr->sin_family = AF_INET;
                printf("Täällä elossa vielä\n");

    serv_addr->sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr->sin_port = htons(port);

    if ((bind(listening_socket, (struct sockaddr *) serv_addr, sizeof(struct sockaddr_in))) < 0) 
        perror("Binding local address");
    listen(listening_socket, 5);
    return listening_socket;
}


