#ifndef CLIENT_H
#define CLIENT_H
#include "connections.h"

/*
 * Parses the users commands and sends them to connect peer. The client
 * does not check whether the client application is connected to 
 * Meeting or a Controller application. The receiving decides what
 * to do with each message
 */
int parse_command(char *message,  Client_state *state);

/*
 * The user interface implementation for the client application. The
 * main idea is to utilize select funtion to monitor user input from
 * STDIN and connected parties by observing their sockets. When user
 * writes a message, it gets passed to the parse_command function. If 
 * data comes from the connected socket, it is written to STDOUT.
 */
void user_interface(char *name);
#endif
