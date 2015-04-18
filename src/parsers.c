#include "parsers.h"
#define BUFFER_LENGTH 300


/*
 * Returns the command code of the message
*/
int parse_command(char *message, int state) {
    struct sockaddr_in server_addr;
    char *temp_message, *next_pointer;
    const char delimiter[2] = " ";

    temp_message = strdup(message);
    next_pointer = strtok(temp_message, delimiter);

    if(strncmp("quitquit", next_pointer, strlen("quitquit")) == 0) {
        printf("quitquit Called\n");
    }
    else if(strncmp("connect", next_pointer, strlen("connect")) == 0) {
        int socket;
        char *ip = strtok(NULL, delimiter);
        char *port = strtok(NULL, delimiter);
        socket = connect_to(ip, atoi(port), &server_addr);
        printf("connect Called\n");
        free(temp_message);
        return socket;
    } 
    else if(strncmp("disconnect", next_pointer, strlen("disconnect")) == 0) {
        printf("disconnect Called\n");
    }
    else if(strncmp("getMeetings", next_pointer, strlen("getMeetings")) == 0) {
        printf("getMeetings Called\n");
    } 
    else if(strncmp("createMeeting", next_pointer, strlen("createMeeting")) == 0) {
        printf("createMeeting Called\n");
    }
    else if(strncmp("talk", next_pointer, strlen("talk")) == 0) {
        printf("talk Called\n");
    }
    else if(strncmp("talkTo", next_pointer, strlen("talkTo")) == 0) {
        printf("talkTo Called\n");
    }
    else if(strncmp("help", next_pointer, strlen("help")) == 0) {
        printf("help Called\n");
    } 
    else {
        printf("Shit happened\n");
    }
    
    free(temp_message);
    return 0;

}

char * parse_message(char *message, int state) {
    char *temp_message, *next_pointer;
    const char delimiter[2] = " ";

    temp_message = strdup(message);
    next_pointer = strtok(temp_message, delimiter);

    switch(1) {
        case 'E' :
            printf("Pid got E \n");
            break;
        case 'C' :
            printf("Pid got C \n");
            break;
        case 'F' :
            printf("Pid got F \n");
            break;
        case 'A' :
            printf("Pid  got A \n");                       
            break;
        case 'Q' :
            printf("Pid got Q \n");           
            break;
        case '#' :
            printf("Pid got C \n");
            break;
        default :
            printf("Pid got command that I didn't understand\n");
            exit(EXIT_FAILURE);
    }
    
    return temp_message;

}
