#include <sys/socket.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <ctype.h>
#include <pthread.h>    

#define BUFFER_LENGTH 300



/*
 * Returns the command code of the message
*/
int parse_command(char *message, int state) {
    char *temp_message, *next_pointer;
    const char delimiter[2] = " ";

    temp_message = strdup(message);
    next_pointer = strtok(temp_message, delimiter);
    printf("Pituus on %d\n", strlen(next_pointer));

    if(strncmp("quitquit", next_pointer, strlen("quitquit")) == 0) {
        printf("quitquit Called\n");
    }
    else if(strncmp("connect", next_pointer, strlen("connect")) == 0) {
        printf("connect Called\n");
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
/*
    while(next_pointer != NULL) {
            printf("String is %s\n", next_pointer);
            next_pointer =  strtok(NULL, delimiter);
    }
  */  
    
    free(temp_message);

}
int test_parse_command() {
    char *test_message1 = "createMeeting Eduskuntavaalit2015";
    char test_message2[100] = "createMeeting Eduskuntavaalit2015";
    char test_message3[100] = "quitquit Eduskuntavaalit2015";
    char test_message4[100] = "Huuhaa";


    //snprintf(test_message2, 33, "createMeeting Eduskuntavaalit2015");
    //test_message2[33]='\0';

    
    char *message = test_message1;
    char *temp;
    printf("This is a test program\n");
    printf("*\n");
    printf("*\n");
    printf("*\n");
    sleep(1);

    printf("Length of the message is %d\n", strlen(message));
    printf("*\n");
    printf("*\n");
    printf("*\n");

    parse_command(test_message2, 3);
    parse_command(test_message3, 3); 
    parse_command(test_message4, 3);


    printf("Goodbye!\n");
    exit(EXIT_SUCCESS);
}

int test_message_creation() {
    char read_buffer[300];
    memset((void *)read_buffer, 0, 300);
    strcat(read_buffer, "22");
    strcat(read_buffer,"01");
    strcat(read_buffer,"0123456789");
    strcat(read_buffer,"This is a meeting   ");
    strcat(read_buffer,"50001");
    printf("Viestin sisältö on %s\n", read_buffer);
}

int create_test_meetings() {
    Meeting_list *my_meetings = (void *) malloc(sizeof(Meeting_list));
    init_meeting_list(my_meetings);
    Meeting *test_meeting = create_new_meeting("Lollotilol", "0123456789", &controller_addr,2);
    Meeting *test_meeting2 = create_new_meeting("Kakkapylly", "2345678943", &controller_addr,0);
    add_meeting_to_list(my_meetings, test_meeting);
    add_meeting_to_list(my_meetings, test_meeting2);

}



int main(int argc, char *argv[]) {

    
    char *s = "01234";
    unsigned int sz = 10;
    char *pad = "0000000000000000000000000000";
    printf ("%.*s%s\n", (sz < strlen(s)) ? 0 : sz - (int)strlen(s), pad, s);


}

