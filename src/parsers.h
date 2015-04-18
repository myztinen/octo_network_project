#ifndef PARSERS_H
#define PARSERS_H
#include "project_defs.h"
#include "connections.h"

#define BUFFER_LENGTH 300

/*
 * Returns the command code of the message
*/
int parse_command(char *message, int state);


char* parse_message(char *message, int state);
#endif
