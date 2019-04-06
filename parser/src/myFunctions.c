/*
Patrick Copp
1007602
CIS 2750
*/
#include "myFunctions.h"

char * alarmToJSON(const Alarm* alarmToMake);

ICalErrorCode firstLineCheck(FILE *fp)
{
  char *firstLine=malloc(75);
  if(firstLine==NULL)
  {
    return OTHER_ERROR;
  }
  if(fp==NULL)
  {
    free(firstLine);
    return INV_FILE;
  }
  fgets(firstLine, 80, fp);

  if(strcmp("BEGIN:VCALENDAR\r\n",firstLine)!=0)
  {
    free(firstLine);
    return INV_CAL;
  }
  else
  {
    free(firstLine);
    return OK;
  }
}

Alarm *AlarmHandler(FILE *fp)
{
  Alarm *newAlarm=malloc(sizeof(Alarm));
  if(newAlarm==NULL)
  {
    return NULL;
  }
  newAlarm->trigger=NULL;
  newAlarm->properties=initializeList(printProperty,deleteProperty,compareProperties);
  bool triggerCheck=false;
  bool actionCheck=false;
  char *currentLine=malloc(80);
  if(currentLine==NULL)
  {
    return NULL;
  }
  char *lineHold=malloc(80);
  if(lineHold==NULL)
  {
    free(currentLine);
    return NULL;
  }
  char *lastLine=malloc(1000);
  if(lastLine==NULL)
  {
    free(currentLine);
    free(lineHold);
    return NULL;
  }
  char *type=NULL;
  char *hold=NULL;
  char delim[]=";:";
  char delim2[]="\r\n";
  while(fgets(currentLine, 80, fp) && strcmp(currentLine,"END:VALARM\r\n")!=0)
  {
    if(currentLine[strlen(currentLine)-2]!='\r' || currentLine[strlen(currentLine)-1]!='\n')
    {
      deleteAlarm(newAlarm);
      free(currentLine);
      free(lineHold);
      free(lastLine);
      return NULL;
    }
    strcpy(lineHold,currentLine);

    //Comment Handler
    if(currentLine[0]==';')
    {
      continue;
    }

    type=strtok(currentLine,delim);

    //ID handler
    if(strcmp(type,"ACTION")==0)
    {
      hold=strtok(NULL,delim2);
      if(hold==NULL)
      {
        deleteAlarm(newAlarm);
        free(currentLine);
        free(lineHold);
        free(lastLine);
        return NULL;
      }
      if(actionCheck)
      {
        deleteAlarm(newAlarm);
        free(currentLine);
        free(lineHold);
        free(lastLine);
        return NULL;
      }

      strcpy(newAlarm->action,hold);
      actionCheck=true;

    }
    else if(strcmp(type,"TRIGGER")==0)
    {
      hold=strtok(NULL,delim2);
      if(hold==NULL)
      {
        free(currentLine);
        free(lineHold);
        free(lastLine);
        return NULL;
      }
      if(triggerCheck)
      {
        deleteAlarm(newAlarm);
        free(currentLine);
        free(lineHold);
        free(lastLine);
        return NULL;
      }
      newAlarm->trigger=malloc(strlen(hold)+1);
      if(newAlarm==NULL)
      {
        free(currentLine);
        free(lineHold);
        free(lastLine);
        return NULL;
      }
      strcpy(newAlarm->trigger,hold);
      triggerCheck=true;

    }

    //General Property Handler
    else
    {
      if(type==NULL)
      {
        free(currentLine);
        free(lineHold);
        free(lastLine);
        return NULL;
      }

      char *hold1=malloc(strlen(type)+1);
      if(hold1==NULL)
      {
        free(currentLine);
        free(lineHold);
        free(lastLine);
        return NULL;
      }
      strcpy(hold1,type);
      type=strtok(NULL,delim2);
      if(type==NULL)
      {
        deleteAlarm(newAlarm);
        free(currentLine);
        free(lineHold);
        free(lastLine);
        return NULL;
      }
      Property *randomProp=malloc(sizeof(Property)+strlen(type)+1);
      if(randomProp==NULL)
      {
        free(currentLine);
        free(lineHold);
        free(lastLine);
        return NULL;
      }
      strcpy(randomProp->propName,hold1);
      strcpy(randomProp->propDescr,type);
      insertBack(newAlarm->properties,randomProp);
      free(hold1);
    }

    strcpy(lastLine,lineHold);

  }

  if(strcmp("END:VALARM\r\n",currentLine)!=0)
  {
    deleteAlarm(newAlarm);
    free(currentLine);
    free(lineHold);
    free(lastLine);
    return NULL;
  }

  if(!actionCheck || !triggerCheck)
  {
    deleteAlarm(newAlarm);
    free(currentLine);
    free(lineHold);
    free(lastLine);
    return NULL;
  }

  free(currentLine);
  free(lineHold);
  free(lastLine);

  return newAlarm;
}

ICalErrorCode writeNewFile(FILE *oldFile)
{
  FILE *newFile=fopen("newFile.ics","w");
  char line[10000];
  int count=0;
  char *token;
  char delims[]={"\r\n"};
  while(fgets(line,10000,oldFile))
  {
    if(line[0]!=' ' && line[0]!='\t' && count!=0)
    {
      fprintf(newFile,"\r\n");
    }
    if(line[strlen(line)-2]!='\r' ||line[strlen(line)-1]!='\n')
    {
      fclose(oldFile);
      fclose(newFile);
      return INV_FILE;
    }
    if(line[0]==' ' || line[0]=='\t')
    {
      memmove(line,line+1,strlen(line));
    }
    token=strtok(line,delims);
    if(token==NULL)
    {
      fclose(oldFile);
      fclose(newFile);
      return INV_FILE;
    }
    fprintf(newFile,"%s",token);
    count++;

  }
  fclose(oldFile);
  fclose(newFile);
  return OK;
}

ICalErrorCode EventHandler2(FILE *fp,Event** newEventHold)
{
  if(fp==NULL)
  {
    return INV_FILE;
  }
  Event *newEvent=malloc(sizeof(Event));
  if(newEvent==NULL)
  {
    return OTHER_ERROR;
  }
  newEvent->properties=initializeList(printProperty,deleteProperty,compareProperties);
  newEvent->alarms=initializeList(printAlarm,deleteAlarm,compareAlarms);

  bool uidCheck=false;
  bool dtstartCheck=false;
  bool dtstampCheck=false;
  char *currentLine=malloc(4000);
  if(currentLine==NULL)
  {
    return OTHER_ERROR;
  }
  char *lineHold=malloc(4000);
  if(lineHold==NULL)
  {
    free(currentLine);
    return OTHER_ERROR;
  }
  char *lastLine=malloc(4000);
  if(lastLine==NULL)
  {
    free(currentLine);
    free(lineHold);
    return OTHER_ERROR;
  }
  char *type=NULL;
  char *hold=NULL;
  char delim[]=";:";
  char delim2[]="\n\r";

  while(fgets(currentLine, 4000, fp) && strcmp(currentLine,"END:VEVENT\r\n")!=0)
  {
    if(currentLine[strlen(currentLine)-2]!='\r' || currentLine[strlen(currentLine)-1]!='\n')
    {
      free(currentLine);
      free(lineHold);
      free(lastLine);
      fclose(fp);
      return INV_FILE;
    }
    strcpy(lineHold,currentLine);

    //Comment Handler
    if(currentLine[0]==';')
    {
      continue;
    }
    //Wrapping handler

    if(strcmp(currentLine,"BEGIN:VALARM\r\n")==0)
    {
      Alarm *newAlarm=AlarmHandler(fp);
      if(newAlarm!=NULL)
      {
        insertBack(newEvent->alarms,newAlarm);
      }
      else
      {
        deleteEvent(newEvent);
        free(currentLine);
        free(lineHold);
        free(lastLine);
        fclose(fp);
        return INV_ALARM;
      }
      continue;
    }

    type=strtok(currentLine,delim);

    //ID handler
    if(strcmp(type,"UID")==0)
    {
      hold=strtok(NULL,delim2);
      if(hold==NULL)
      {
        deleteEvent(newEvent);
        free(currentLine);
        free(lineHold);
        free(lastLine);
        fclose(fp);
        return INV_EVENT;
      }
      if(uidCheck)
      {
        deleteEvent(newEvent);
        free(currentLine);
        free(lineHold);
        free(lastLine);
        fclose(fp);
        return INV_EVENT;
      }
      strcpy(newEvent->UID,hold);
      uidCheck=true;

    }
    //DTSTART handler
    else if(strcmp(type,"DTSTART")==0)
    {

      hold=strtok(NULL,delim2);
      if(hold==NULL)
      {
        deleteEvent(newEvent);
        free(currentLine);
        free(lineHold);
        free(lastLine);
        fclose(fp);
        return INV_EVENT;
      }
      if(dtstartCheck)
      {
        deleteEvent(newEvent);
        free(currentLine);
        free(lineHold);
        free(lastLine);
        fclose(fp);
        return INV_EVENT;
      }
      if(strlen(hold)!=15 && strlen(hold)!=16)
      {
        deleteEvent(newEvent);
        free(currentLine);
        free(lineHold);
        free(lastLine);
        fclose(fp);
        return INV_DT;
      }


      if(hold[strlen(hold)-1]=='Z')
      {
        newEvent->startDateTime.UTC=true;
        hold[strlen(hold)-1]='\0';
      }
      else
      {
        newEvent->startDateTime.UTC=false;
      }
      hold=strtok(hold,"T");
      strcpy(newEvent->startDateTime.date,hold);
      hold=strtok(NULL,delim2);
      if(hold==NULL)
      {
        deleteEvent(newEvent);
        free(currentLine);
        free(lineHold);
        free(lastLine);
        fclose(fp);
        return INV_DT;
      }
      strcpy(newEvent->startDateTime.time,hold);

      dtstartCheck=true;
    }
    //DTSTAMP handler
    else if(strcmp(type,"DTSTAMP")==0)
    {
      hold=strtok(NULL,delim2);
      if(hold==NULL)
      {
        deleteEvent(newEvent);
        free(currentLine);
        free(lineHold);
        free(lastLine);
        fclose(fp);
        return INV_EVENT;
      }
      if(dtstampCheck)
      {
        deleteEvent(newEvent);
        free(currentLine);
        free(lineHold);
        free(lastLine);
        fclose(fp);
        return INV_EVENT;
      }
      if(strlen(hold)!=15 && strlen(hold)!=16)
      {
        deleteEvent(newEvent);
        free(currentLine);
        free(lineHold);
        free(lastLine);
        fclose(fp);
        return INV_DT;
      }
      if(hold[strlen(hold)-1]=='Z')
      {
        newEvent->creationDateTime.UTC=true;
        hold[strlen(hold)-1]='\0';
      }
      else
      {
        newEvent->creationDateTime.UTC=false;
      }
      hold=strtok(hold,"T");
      if(hold==NULL)
      {
        deleteEvent(newEvent);
        free(currentLine);
        free(lineHold);
        free(lastLine);
        fclose(fp);
        return INV_DT;
      }
      strcpy(newEvent->creationDateTime.date,hold);
      hold=strtok(NULL,delim2);
      if(hold==NULL)
      {
        deleteEvent(newEvent);
        free(currentLine);
        free(lineHold);
        free(lastLine);
        fclose(fp);
        return INV_DT;
      }
      strcpy(newEvent->creationDateTime.time,hold);
      dtstampCheck=true;
    }

    else if(strcmp(type,"BEGIN")==0)
    {
      deleteEvent(newEvent);
      free(currentLine);
      free(lineHold);
      free(lastLine);
      fclose(fp);
      return INV_EVENT;
    }
    //General Property Handler
    else
    {
      if(type==NULL)
      {
        deleteEvent(newEvent);
        free(currentLine);
        free(lineHold);
        free(lastLine);
        fclose(fp);
        return INV_EVENT;
      }

      char *hold1=malloc(strlen(type)+1);
      if(hold1==NULL)
      {
        deleteEvent(newEvent);
        free(currentLine);
        free(lineHold);
        free(lastLine);
        fclose(fp);
        return OTHER_ERROR;
      }
      strcpy(hold1,type);
      type=strtok(NULL,delim2);
      if(type==NULL)
      {
        free(hold1);
        deleteEvent(newEvent);
        free(currentLine);
        free(lineHold);
        free(lastLine);
        fclose(fp);
        return INV_EVENT;
      }
      Property *randomProp=malloc(sizeof(Property)+strlen(type)+1);
      if(randomProp==NULL)
      {
        deleteEvent(newEvent);
        free(currentLine);
        free(lineHold);
        free(lastLine);
        fclose(fp);
        return OTHER_ERROR;
      }
      strcpy(randomProp->propName,hold1);
      strcpy(randomProp->propDescr,type);
      insertBack(newEvent->properties,randomProp);
      free(hold1);
    }

    strcpy(lastLine,lineHold);

  }

  if(!uidCheck || !dtstartCheck || !dtstampCheck)
  {
    deleteEvent(newEvent);
    free(currentLine);
    free(lineHold);
    free(lastLine);
    fclose(fp);
    return INV_EVENT;
  }
  if(strcmp(currentLine,"END:VEVENT\r\n")!=0)
  {
    deleteEvent(newEvent);
    free(currentLine);
    free(lineHold);
    free(lastLine);
    fclose(fp);
    return INV_EVENT;
  }

  free(currentLine);
  free(lineHold);
  free(lastLine);
  *newEventHold=newEvent;
  return OK;
}

char * testFunction()
{
  char * hi=malloc(10);
  strcpy(hi,"go fuck");
  return hi;
}

void testFunction2(char * hi)
{
  return;
}

char * fileToJSON(char * fileName)
{
  Calendar *newCalendar;
  ICalErrorCode error=createCalendar(fileName,&newCalendar);
  if(error!=OK)
  {
    return "{}";
  }

  char *toReturn=malloc(200);
  strcpy(toReturn,"{\"filename\":\"");
  strcat(toReturn,fileName);
  strcat(toReturn,"\",");
  char * toReturn2=calendarToJSON(newCalendar);
  memmove(toReturn2, toReturn2+ 1, strlen(toReturn2));
  strcat(toReturn,toReturn2);
  return toReturn;

}

char * simpleToCal(char * filename, char * prodID, char * version,char * eventUID,char * eventDTSTAMP,char * eventDTSTART)
{

  Calendar * newCal=malloc(sizeof(Calendar));
  strcpy(newCal->prodID,prodID);
  newCal->version=atof(version);
  newCal->properties=initializeList(printProperty,deleteProperty,compareProperties);
  newCal->events=initializeList(printEvent,deleteEvent,compareEvents);

  Event * newEvent=malloc(sizeof(Event));

  newEvent->properties=initializeList(printProperty,deleteProperty,compareProperties);
  newEvent->alarms=initializeList(printAlarm,deleteAlarm,compareAlarms);
  strcpy(newEvent->UID,eventUID);



  if(strlen(eventDTSTART)!=15 && strlen(eventDTSTART)!=16)
  {
    return "INVALID DT";
  }

  if(eventDTSTART[strlen(eventDTSTART)-1]=='Z')
  {
    newEvent->startDateTime.UTC=true;
    eventDTSTART[strlen(eventDTSTART)-1]='\0';
  }
  else
  {
    newEvent->startDateTime.UTC=false;
  }

  char * hold=strtok(eventDTSTART,"T");
  if(hold==NULL)
  {
    return "INVALID DT";
  }
  strcpy(newEvent->startDateTime.date,hold);
  hold=strtok(NULL,"\r\n\0");
  if(hold==NULL)
  {
    return "INVALID DT";
  }
  strcpy(newEvent->startDateTime.time,hold);


  char *hold2;

  if(strlen(eventDTSTAMP)!=15 && strlen(eventDTSTAMP)!=16)
  {
    return "INVALID DT";
  }

  if(eventDTSTAMP[strlen(eventDTSTAMP)-1]=='Z')
  {
    newEvent->creationDateTime.UTC=true;
    eventDTSTAMP[strlen(eventDTSTAMP)-1]='\0';
  }
  else
  {
    newEvent->creationDateTime.UTC=false;
  }

  hold2=strtok(eventDTSTAMP,"T");
  if(hold2==NULL)
  {
    return "INVALID DT";
  }
  strcpy(newEvent->creationDateTime.date,hold2);
  hold2=strtok(NULL,"\0");
  if(hold2==NULL)
  {
    return "INVALID DT";
  }
  strcpy(newEvent->creationDateTime.time,hold2);

  insertBack(newCal->events,newEvent);

  ICalErrorCode error=validateCalendar(newCal);
  if(error!=OK)
  {
    return "Could not validate Calendar";
  }
  error=writeCalendar(filename,newCal);

  if(error==OK)
  {
    char *hold=calendarToJSON(newCal);
    deleteCalendar(newCal);
    return hold;
  }
  return "Error writing new calendar";
}

char * fileToJSONEvents(char * fileName)
{
  Calendar * newCal;
  ICalErrorCode error=createCalendar(fileName,&newCal);
  if(error!=OK)
  {
    printf("%s\n",printError(error));
    return "ERROR WITH FILE";
  }
  char * eventList = eventListToJSON(newCal->events);
  return eventList;
}

char * fileToNewEvent(char * fileName,char *uid,char *summary,char *dtstart,char *dtstamp)
{
  Calendar * addTo;
  ICalErrorCode error=createCalendar(fileName,&addTo);
  if(error!=OK)
  {
    return printError(error);
  }
  Event * newEvent=malloc(sizeof(Event));

  newEvent->properties=initializeList(printProperty,deleteProperty,compareProperties);
  newEvent->alarms=initializeList(printAlarm,deleteAlarm,compareAlarms);
  strcpy(newEvent->UID,uid);
  if(strcmp(summary,"")!=0)
  {
    Property *randomProp=malloc(sizeof(Property)+strlen(summary)+1);
    strcpy(randomProp->propName,"SUMMARY");
    strcpy(randomProp->propDescr,summary);
    insertBack(newEvent->properties,randomProp);
  }


  if(strlen(dtstart)!=15 && strlen(dtstart)!=16)
  {
    return "INVALID DT";
  }

  if(dtstart[strlen(dtstart)-1]=='Z')
  {
    newEvent->startDateTime.UTC=true;
    dtstart[strlen(dtstart)-1]='\0';
  }
  else
  {
    newEvent->startDateTime.UTC=false;
  }
  char * hold=strtok(dtstart,"T");
  if(hold==NULL)
  {
    return "INVALID DT";
  }
  strcpy(newEvent->startDateTime.date,hold);
  hold=strtok(NULL,"\r\n\0");
  if(hold==NULL)
  {
    return "INVALID DT";
  }
  strcpy(newEvent->startDateTime.time,hold);

  char *hold2;

  if(strlen(dtstamp)!=15 && strlen(dtstamp)!=16)
  {
    return "INVALID DT";
  }

  if(dtstamp[strlen(dtstamp)-1]=='Z')
  {
    newEvent->creationDateTime.UTC=true;
    dtstamp[strlen(dtstamp)-1]='\0';
  }
  else
  {
    newEvent->creationDateTime.UTC=false;
  }

  hold2=strtok(dtstamp,"T");
  if(hold2==NULL)
  {
    return "INVALID DT";
  }
  strcpy(newEvent->creationDateTime.date,hold2);
  hold2=strtok(NULL,"\0");
  if(hold2==NULL)
  {
    return "INVALID DT";
  }
  strcpy(newEvent->creationDateTime.time,hold2);

  insertBack(addTo->events,newEvent);


  error=validateCalendar(addTo);
  if(error!=OK)
  {
    return "Could not validate Calendar";
  }


  error=writeCalendar(fileName,addTo);

  if(error==OK)
  {
    deleteCalendar(addTo);
    return "worked";
  }
  else
  {
    return "yikes";
  }
}

char * fileToAllAlarms(char * fileName,char * eventNumber)
{
  int number=atoi(eventNumber);
  Calendar * addTo;
  ICalErrorCode error=createCalendar(fileName,&addTo);
  if(error!=OK)
  {
    return "Error accessing file.";
  }
  ListIterator events = createIterator(addTo->events);
  Event *eventHolder=nextElement(&events);
  int count=0;
  number--;
  while(eventHolder!=NULL)
  {
    if(count==number)
    {
      break;
    }
    eventHolder=nextElement(&events);
    count++;
  }

  char * toReturn=malloc(10000);

  ListIterator eventAlarms = createIterator(eventHolder->alarms);
  Alarm *alarmHolder=nextElement(&eventAlarms);
  if(alarmHolder==NULL)
  {
    return "No alarms.";
  }
  strcpy(toReturn,printAlarm(alarmHolder));

  while(alarmHolder!=NULL)
  {
    alarmHolder=nextElement(&eventAlarms);
    if(alarmHolder==NULL)
    {
      break;
    }
    strcat(toReturn,printAlarm(alarmHolder));
    strcat(toReturn,"\n\n\n");
  }

  return toReturn;
}

char * fileToAllProps(char * fileName,char * eventNumber)
{
  int number=atoi(eventNumber);
  Calendar * addTo;
  ICalErrorCode error=createCalendar(fileName,&addTo);
  if(error!=OK)
  {
    return "Error accessing file.";
  }
  ListIterator events = createIterator(addTo->events);
  Event *eventHolder=nextElement(&events);
  int count=0;
  number--;
  while(eventHolder!=NULL)
  {
    if(count==number)
    {
      break;
    }
    eventHolder=nextElement(&events);
    count++;
  }

  char * toReturn=malloc(10000);

  ListIterator eventAlarms = createIterator(eventHolder->properties);
  Property *propHolder=nextElement(&eventAlarms);
  strcpy(toReturn,"UID: ");
  strcat(toReturn,eventHolder->UID);
  strcat(toReturn,"\nDTSTAMP:");
  strcat(toReturn,printDate(&(eventHolder->startDateTime)));
  strcat(toReturn,"\nDTSTART:");
  strcat(toReturn,printDate(&(eventHolder->startDateTime)));
  strcat(toReturn,"\n");
  if(propHolder==NULL)
  {
    return toReturn;
  }
  strcat(toReturn,printProperty(propHolder));
  while(propHolder!=NULL)
  {
    propHolder=nextElement(&eventAlarms);
    if(propHolder==NULL)
    {
      break;
    }
    char *hello=printProperty(propHolder);
    strcat(toReturn,hello);
    strcat(toReturn,"\n\n\n");
  }
  return toReturn;
}

char * fileToJSONAlarms(char * fileName,char * eventNumber)
{
  int number=atoi(eventNumber);
  Calendar * addTo;
  ICalErrorCode error=createCalendar(fileName,&addTo);
  if(error!=OK)
  {
    printf("Error accessing file.\n");
    return "{}";
  }
  ListIterator events = createIterator(addTo->events);
  Event *eventHolder=nextElement(&events);
  int count=0;

  while(eventHolder!=NULL)
  {
    if(count==number)
    {
      break;
    }
    eventHolder=nextElement(&events);
    count++;
  }
  if(eventHolder==NULL)
  {
    return "{}";
  }

  char * toReturn=malloc(10000);
  ListIterator eventAlarms = createIterator(eventHolder->alarms);

  Alarm *alarmHolder=nextElement(&eventAlarms);
  if(alarmHolder==NULL)
  {
    return "{}";
  }
  strcpy(toReturn,"[");
  char * holder;
  while(alarmHolder!=NULL)
  {
    holder=alarmToJSON(alarmHolder);
    strcat(toReturn,holder);
    free(holder);
    alarmHolder = nextElement(&eventAlarms);
    if(alarmHolder!=NULL)
    {
      strcat(toReturn,",");
    }
  }
  strcat(toReturn,"]");

  return toReturn;
}

char * alarmToJSON(const Alarm* alarmToMake)
{
  char * toReturn=malloc(1000);
  strcpy(toReturn,"{\"action\":\"");
  strcat(toReturn,alarmToMake->action);
  strcat(toReturn,"\",\"trigger\":\"");
  strcat(toReturn,alarmToMake->trigger);
  strcat(toReturn,"\"}");
  return toReturn;
}












//dsds
