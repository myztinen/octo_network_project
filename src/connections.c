#include "connections.h"

int safe_write(int socket, const char *buffer, int len) {
    int write_amount = 0;
    if((write_amount = write(socket, buffer, len)) < 0) {
        perror("Error in writing. Global errno is ");
        write(STDOUT_FILENO, &errno, 1);
        write(STDOUT_FILENO, "\n", 1);

        exit(EXIT_FAILURE);
    }
    //memset((void *) buffer, 0, len);
    return write_amount;
}

int safe_read(int socket, char *buffer, int len) {
    memset((void *) buffer, 0, len);
    int read_amount = 0;
    if((read_amount = read(socket, buffer, len)) < 0) {
        perror("Error in reading. Global errno is ");
        write(STDOUT_FILENO, &errno, 1);
        write(STDOUT_FILENO, "\n", 1);
        exit(EXIT_FAILURE);
    }
 
    return read_amount;
}

int add_to_buffer(char *buffer, const char *addative, int field_size) {
    char temp[field_size+1];
    char *padding = "0000000000000000000000000000";
    sprintf(temp, "%.*s%s", (field_size < strlen(addative)) ? 0 : field_size - (int)strlen(addative), padding, addative);

    strncat(buffer, temp, field_size);
    strncat(buffer, " ", 1); 
    return field_size+1;
}

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
    const int on = 1;
    int listening_socket;
    if ( (listening_socket = socket(AF_INET, SOCK_STREAM, PF_UNSPEC)) < 0) {
        perror("Creating stream socket");
        exit(EXIT_FAILURE);
    }

    serv_addr->sin_family = AF_INET;
    serv_addr->sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr->sin_port = htons(port);
    setsockopt(listening_socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int));

    if ((bind(listening_socket, (struct sockaddr *) serv_addr, sizeof(struct sockaddr_in))) < 0) {
        perror("Binding local address");
        exit(EXIT_FAILURE);
    }
    listen(listening_socket, 5);
    return listening_socket;
}

int create_HELLO_message(char *buffer, int buffer_len) {
    int message_len = 0;
    memset((void *)buffer, 0, buffer_len);
    message_len += add_to_buffer(buffer, HELLO, 2);
    printf("Viestin sisältö on %s\n", buffer);
    return message_len;
}

int create_IAMSERVER_message(char *buffer, int buffer_len, const char *server_id) {
    int message_len = 0;
    memset((void *)buffer, 0, buffer_len);
    message_len += add_to_buffer(buffer, IAMSERVER, 2);

    if(strlen(server_id) != 10) {
        perror("server id is not valid length!!\n");
        exit(EXIT_FAILURE);
    }

    message_len += add_to_buffer(buffer, server_id, 10);
    return message_len;
}
int create_IAMCLIENT_message(char *buffer, int buffer_len, const char *server_id) {
    int message_len = 0;
    memset((void *)buffer, 0, buffer_len);
    message_len += add_to_buffer(buffer, IAMCLIENT, 2);

    if(strlen(server_id) > 10 || strlen(server_id) < 3) {
        perror("server id is not valid length!!\n");
        exit(EXIT_FAILURE);
    }

    message_len += add_to_buffer(buffer, server_id, 10);
    return message_len;
}


int create_LISTOFMEETINGS_SERVER_message(char *buffer, int buffer_len, Meeting_server *server) {
    Meeting *iter = server->head;
    int message_len = 0;
    char list_amount[3];
    char participants[3];
    char meeting_port[6];
    snprintf(list_amount, 2, "%d",  server->amount_of_meetings);
    
    memset((void *)buffer, 0, buffer_len);
    message_len += add_to_buffer(buffer, LISTOFMEETINGS_SERVER, 2);
    message_len += add_to_buffer(buffer, server->server_id, 10);
    message_len += add_to_buffer(buffer, list_amount, 2);
    if(server->amount_of_meetings == 0) return message_len;
    while(iter != NULL) {
        memset((void *) meeting_port, 0, 6);
        snprintf(meeting_port, 6, "%d",  iter->port);
        
        message_len += add_to_buffer(buffer, iter->meeting_id,10);
        message_len += add_to_buffer(buffer, iter->meeting_topic, 20);
        message_len += add_to_buffer(buffer, meeting_port,5);
        snprintf(participants, 2, "%d",  iter->participant_amount);
        message_len += add_to_buffer(buffer, participants,2);
        iter = iter->next;
    }
    return message_len;
}

int create_LISTOFMEETINGS_CONTROLLER_message(char *buffer, int buffer_len, Meeting_server_list *list) {
    Meeting_server *serv_iter =list->head;
    Meeting *iter = serv_iter->head;
    int message_len = 0;
    char list_amount[3];
    char participants[3];
    char meeting_port[6];
    snprintf(list_amount, 2, "%d",  list->amount);
    
    memset((void *)buffer, 0, buffer_len);
    message_len += add_to_buffer(buffer, LISTOFMEETINGS_CONTROLLER, 2);
    message_len += add_to_buffer(buffer, list_amount, 2);
    if(serv_iter->amount_of_meetings == 0) return message_len;
    while(serv_iter != NULL) {
        if(serv_iter->head != NULL) {
            while(iter != NULL) {
                memset((void *) meeting_port, 0, 6);
                snprintf(meeting_port, 6, "%d",  ntohs(iter->port));
                message_len += add_to_buffer(buffer, iter->meeting_id,10);
                message_len += add_to_buffer(buffer, iter->meeting_topic, 20);
                message_len += add_to_buffer(buffer, meeting_port,5);
                snprintf(participants, 2, "%d",  iter->participant_amount);
                message_len += add_to_buffer(buffer, participants,2);
                message_len += add_to_buffer(buffer, participants,2);
                strncat(buffer, "\n", 1); 
                iter = iter->next;
            }
        }
        serv_iter = serv_iter->next;
    }
    return message_len;
}

int create_CREATENEWMEETING_CLIENT_message(char *buffer, int buffer_len,  char *topic) {
    int message_len = 0;
    memset((void *)buffer, 0, buffer_len);
    message_len += add_to_buffer(buffer, CREATENEWMEETING_CLIENT, 2);

    if(strlen(topic) > 20 || strlen(topic) < 1) {
        perror("topic is not valid length!!\n");
        memset((void *)buffer, 0, buffer_len);
        return 0;
    }
    message_len += add_to_buffer(buffer, topic, 20);
    return message_len;
}

int create_CREATENEWMEETING_CONTROLLER_message(char *buffer, int buffer_len,  char *topic) {
    int message_len = 0;
    memset((void *)buffer, 0, buffer_len);
    message_len += add_to_buffer(buffer, CREATENEWMEETING_CONTROLLER, 2);

    if(strlen(topic) > 20 || strlen(topic) < 1) {
        perror("topic is not valid length!!\n");
        memset((void *)buffer, 0, buffer_len);
        return 0;
    }
    message_len += add_to_buffer(buffer, topic, 20);
    return message_len;
}

int create_QUIT_message(char *buffer, int buffer_len) {
    int message_len = 0;
    memset((void *)buffer, 0, buffer_len);
    message_len += add_to_buffer(buffer, QUIT, 2);
    return message_len;
}

int create_GETDISCUSSIONS_message(char *buffer, Meeting_server_list *list, int buffer_len) {
    memset((void *)buffer, 0, buffer_len);
    int message_len = 0;
    char address[INET_ADDRSTRLEN];
    char meeting_port[6];
    Meeting *meeting_iter;
    Meeting_server *server_iter = list->head;
    if(server_iter == NULL) {
        message_len += add_to_buffer(buffer, "No meeting servers available!\n",30);
        return message_len;
    }
    meeting_iter = server_iter->head;

    while(server_iter != NULL) {
        meeting_iter = server_iter->head;
        if(meeting_iter == NULL && message_len == 0) {
            message_len += add_to_buffer(buffer, "No meetings available! You create one if you like.\n",51);
            return message_len;
        }
        while(meeting_iter != NULL) {
            message_len += add_to_buffer(buffer, "Topic:",6);
            message_len += add_to_buffer(buffer, meeting_iter->meeting_topic,20);
            message_len += add_to_buffer(buffer, "IP:",3);
            inet_ntop(AF_INET, &(server_iter->server_address.sin_addr), address, INET_ADDRSTRLEN);
            message_len += add_to_buffer(buffer, address,INET_ADDRSTRLEN);
            message_len += add_to_buffer(buffer, "Port:",5);
            snprintf(meeting_port, 6, "%d",  meeting_iter->port);
            message_len += add_to_buffer(buffer, meeting_port,5);
            strncat(buffer, "\n", 1);
            message_len += 1;
            meeting_iter = meeting_iter->next;
        }
    server_iter = server_iter->next;
    }
    return message_len;
}

int create_TALK_message(char *message, Client_state *state, char *buffer, int buffer_len) {
    memset((void *)buffer, 0, buffer_len);
    int message_len = 0;
    message_len += add_to_buffer(buffer, state->client_id, strlen(state->client_id));
    strncat(buffer, ">>> ", 4);
    message_len += 4;
    message_len += add_to_buffer(buffer, message, strlen(message));
    return message_len;
}



    

    
