#include "project_defs.h"

const char *HELLO = "01";
const char *ACK = "02";
const char *NACK = "03";
const char *QUIT = "04";

const char *IAMCLIENT = "10";
const char *GETDISCUSSIONS = "11";
const char *CREATENEWMEETING_CLIENT = "12";
const char *GETLISTOFPARTICIPANTS = "13";
const char *TALK = "14";
const char *TALKTO = "15";

const char *IAMSERVER = "20";
const char *MEETINGINFORMATION = "21";
const char *LISTOFMEETINGS_SERVER = "22";
const char *MEETINGTERMINATED = "23";

const char *CREATENEWMEETING_CONTROLLER = "30";
const char *LISTOFMEETINGS_CONTROLLER = "31";

const char *HELPTEXT = "****************************************\n*          CLIENT APPLICATION          *\n*      Commands:                       *\n*       connect <ip address> <port>    *\n*       quitquit                       *\n*       help                           *\n*       talk <message>                 *\n*       getMeetings                    *\n*       createMeeting <meeting topic>  *\n*       exit                           *\n****************************************\n";


