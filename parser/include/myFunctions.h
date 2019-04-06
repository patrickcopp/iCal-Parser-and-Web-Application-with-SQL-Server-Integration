/*
Patrick Copp
1007602
CIS 2750
*/
#ifndef MYFUNCTIONS_H
#define MYFUNCTIONS_H

#include <stdlib.h>
#include <string.h>

#include "CalendarParser.h"
#include "LinkedListAPI.h"
ICalErrorCode firstLineCheck(FILE *);
Alarm *AlarmHandler(FILE *);
ICalErrorCode EventHandler2(FILE *,Event**);
ICalErrorCode writeNewFile(FILE *);
char * testFunction();
void testFunction2(char *);
char * fileToJSON(char *);
char * simpleToCal(char * filename, char * prodID, char * version,char * eventUID,char * eventDTSTAMP,char * eventDTSTART);

#endif
