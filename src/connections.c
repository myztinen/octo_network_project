#include "connections.h"

int add_to_buffer(char *buffer, const char *addative) {
    int addative_length = strlen(addative);
    strncat(buffer, addative, addative_length);
    return addative_length;
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

int create_HELLO_message(char *buffer, int buffer_len) {
    int message_len = 0;
    memset((void *)buffer, 0, buffer_len);
    message_len += add_to_buffer(buffer, HELLO);
    printf("Viestin sisältö on %s\n", read_buffer);
    return message_len;
}

int create_IAMSERVER_message(char *buffer, int buffer_len, const char *server_id) {
    int message_len = 0;
    memset((void *)buffer, 0, buffer_len);
    message_len += add_to_buffer(buffer, IAMSERVER);
    if(strlen(server_id) != 10) {
        perror("server id is not valid length!!)
        exit(EXIT_FAILURE);
    }
    message_len += add_to_buffer(buffer, IAMSERVER);
    printf("Viestin sisältö on %s\n", read_buffer);
    return message_len;
}

/*
 * Command Code Char 2 Value is 22
 * ListLength Char 2 Number of meetings in this message. Maximum value is 20
* MeetingId Char 10 Random value MeetingTopic Char 20 Minimum length is 1
* Port Number Char 5
* 
* 
* typedef struct meeting {
    char meetingName[20];
    char serverName[20];
    struct sockaddr_in meeting_server;
    struct sockaddr_in meeting_address;
    int participant_amount;
    struct meeting *next;
    } Meeting;

    
typedef struct meeting_list{
    int amount;
    Meeting *head;
    } Meeting_list;

*/

int create_LISTOFMEETINGS_SERVER_message(char *buffer, int buffer_len, Meeting_list *list) {
    int message_len = 0;
    char list_amount[3];
    char meeting_port[6];
    snprintf(list_amount, 3, "%d",  list->amount);
    snprintf(meeting_port, 6, "%d",  ntohs(iter->meeting_address->sin_port));
    

    memset((void *)buffer, 0, buffer_len);
    message_len += add_to_buffer(buffer, LISTOFMEETINGS_SERVER);
    message_len += add_to_buffer(buffer, list_amount);
    if(list->amount == 0) return message_len;
    Meeting *iter = list->head;
    while(iter != NULL) {
        message_len += add_to_buffer(buffer, iter->meeting_id);
        message_len += add_to_buffer(buffer, iter->meeting_topic);
        message_len += add_to_buffer(buffer, meeting_port);
        iter = iter->next;
    }
    return message_len;
}

        




    message_len += add_to_buffer(buffer, IAMSERVER);
    printf("Viestin sisältö on %s\n", read_buffer);
    return message_len;
}

