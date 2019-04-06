/*
Patrick Copp
1007602
CIS 2750
*/
#include "CalendarParser.h"
#include "myFunctions.h"

ICalErrorCode createCalendar(char* fileName, Calendar** obj)
{

  if(fileName==NULL)
  {
    return INV_FILE;
  }
  if(strlen(fileName)<5 || fileName[strlen(fileName)-4]!='.'|| fileName[strlen(fileName)-3]!='i'|| fileName[strlen(fileName)-2]!='c'|| fileName[strlen(fileName)-1]!='s')
  {
    return INV_FILE;
  }
  bool versionCheck=false;
  bool prodidCheck=false;
  bool calscaleCheck=false;
  bool methodCheck=false;

  *obj=malloc(sizeof(Calendar));
  if(*obj==NULL)
  {
    return OTHER_ERROR;
  }

  //Initializes Properties List
  (*obj)->properties=initializeList(printProperty,deleteProperty,compareProperties);
  (*obj)->events=initializeList(printEvent,deleteEvent,compareEvents);
  FILE *fp;
  fp=fopen(fileName, "r");
  if(fp==NULL)
  {
    deleteCalendar(*obj);
    *obj=NULL;
    return INV_FILE;
  }

  ICalErrorCode hi=writeNewFile(fp);
  if(hi!=OK)
  {
    return INV_FILE;
  }
  fp=fopen("newFile.ics","r");

  /*Checks if file pointer is valid and makes sure first line is BEGIN:VCALENDAR*/
  ICalErrorCode firstCheck=firstLineCheck(fp);
  if(firstCheck!=OK)
  {
    fclose(fp);
    deleteCalendar(*obj);
    *obj=NULL;
    return firstCheck;
  }

  char *currentLine=malloc(4000);
  if(currentLine==NULL)
  {
    deleteCalendar(*obj);
    *obj=NULL;
    return OTHER_ERROR;
  }
  char *lineHold=malloc(4000);
  if(lineHold==NULL)
  {
    free(currentLine);
    deleteCalendar(*obj);
    *obj=NULL;
    return OTHER_ERROR;
  }
  char *lastLine=malloc(1000);
  if(lastLine==NULL)
  {
    free(currentLine);
    free(lineHold);
    deleteCalendar(*obj);
    *obj=NULL;
    return OTHER_ERROR;
  }

  char *type=NULL;
  char *hold;
  char delim[]=";:";
  char delim2[]="\n\r";

  while(fgets(currentLine, 4000, fp) && strncmp("END:VCALENDAR",currentLine,13)!=0)
  {
    if(currentLine[strlen(currentLine)-2]!='\r' || currentLine[strlen(currentLine)-1]!='\n')
    {
      deleteCalendar(*obj);
      *obj=NULL;
      free(currentLine);
      free(lineHold);
      free(lastLine);
      fclose(fp);
      return INV_FILE;
    }
    strcpy(lineHold,currentLine);
    if(currentLine[0]==';')
    {
      continue;
    }
    /*Wrapping handler*/

    if(strncmp(currentLine,"BEGIN:VEVENT",12)==0)
    {
      Event *newEvent;
      ICalErrorCode error=EventHandler2(fp,&newEvent);
      if(error!=OK)
      {
        deleteCalendar(*obj);
        *obj=NULL;
        free(lineHold);
        free(currentLine);
        free(lastLine);
        return error;
      }
      insertFront((*obj)->events,newEvent);
      continue;
    }

    type=strtok(currentLine,delim);
    /*Version handler*/
    if(strcmp(type,"VERSION")==0)
    {
      hold=strtok(NULL,delim2);
      if(hold==NULL)
      {
        deleteCalendar(*obj);
        *obj=NULL;
        free(currentLine);
        free(lineHold);
        free(lastLine);
        fclose(fp);
        return INV_VER;
      }

      if(versionCheck==true)
      {
        deleteCalendar(*obj);
        *obj=NULL;
        free(currentLine);
        free(lineHold);
        free(lastLine);
        fclose(fp);
        return DUP_VER;
      }

      versionCheck=true;
      sscanf(hold,"%f",&(*obj)->version);
    }
    /*ProductID handler*/
    else if(strcmp(type,"PRODID")==0)
    {
      hold=strtok(NULL,delim2);
      if(hold==NULL)
      {
        deleteCalendar(*obj);
        *obj=NULL;
        free(lineHold);
        free(currentLine);
        free(lastLine);
        fclose(fp);
        return INV_PRODID;
      }
      if(prodidCheck)
      {
        deleteCalendar(*obj);
        *obj=NULL;
        free(lineHold);
        free(currentLine);
        free(lastLine);
        fclose(fp);
        return DUP_PRODID;
      }
      prodidCheck=true;
      strcpy((*obj)->prodID,hold);
    }
    //Calscale handler
    else if(strcmp(type,"CALSCALE")==0)
    {
      if(calscaleCheck)
      {
        deleteCalendar(*obj);
        *obj=NULL;
        free(lineHold);
        free(currentLine);
        free(lastLine);
        fclose(fp);
        return INV_CAL;
      }

      type=strtok(NULL,delim2);

      if(type==NULL)
      {
        deleteCalendar(*obj);
        *obj=NULL;
        free(lineHold);
        free(currentLine);
        free(lastLine);
        fclose(fp);
        return INV_CAL;
      }

      Property *calscaleProp=malloc(sizeof(Property)+strlen(type)+1);
      if(calscaleProp==NULL)
      {
        deleteCalendar(*obj);
        *obj=NULL;
        free(currentLine);
        free(lineHold);
        free(lastLine);
        fclose(fp);
        return OTHER_ERROR;
      }
      strcpy(calscaleProp->propName,"CALSCALE");
      strcpy(calscaleProp->propDescr,type);
      insertBack((*obj)->properties,calscaleProp);
      calscaleCheck=true;
    }
    //Method handler
    else if(strcmp(type,"METHOD")==0)
    {

      if(methodCheck)
      {
        deleteCalendar(*obj);
        *obj=NULL;
        free(currentLine);
        free(lineHold);
        free(lastLine);
        fclose(fp);
        return INV_CAL;
      }

      type=strtok(NULL,delim2);

      if(type==NULL)
      {
        deleteCalendar(*obj);
        *obj=NULL;
        free(lineHold);
        free(currentLine);
        free(lastLine);
        fclose(fp);
        return INV_FILE;
      }

      Property *methodProp=malloc(sizeof(Property)+strlen(type)+1);
      if(methodProp==NULL)
      {
        deleteCalendar(*obj);
        *obj=NULL;
        free(currentLine);
        free(lineHold);
        free(lastLine);
        fclose(fp);
        return OTHER_ERROR;
      }
      strcpy(methodProp->propName,"METHOD");
      strcpy(methodProp->propDescr,type);
      insertBack((*obj)->properties,methodProp);
      methodCheck=true;
    }

    else if(strcmp(type,"BEGIN")==0)
    {
      deleteCalendar(*obj);
      *obj=NULL;
      free(currentLine);
      free(lineHold);
      free(lastLine);
      fclose(fp);
      return INV_FILE;
    }

    //General Property Handler
    else
    {
      if(type==NULL)
      {
        deleteCalendar(*obj);
        *obj=NULL;
        free(currentLine);
        free(lineHold);
        free(lastLine);
        fclose(fp);
        return INV_FILE;
      }

      char *hold1=malloc(strlen(type)+1);
      if(hold1==NULL)
      {
        deleteCalendar(*obj);
        *obj=NULL;
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
        deleteCalendar(*obj);
        *obj=NULL;
        free(hold1);
        free(currentLine);
        free(lineHold);
        free(lastLine);
        fclose(fp);
        return INV_FILE;
      }
      Property *randomProp=malloc(sizeof(Property)+strlen(type)+1);
      if(randomProp==NULL)
      {
        deleteCalendar(*obj);
        *obj=NULL;
        free(currentLine);
        free(lineHold);
        free(lastLine);
        fclose(fp);
        return OTHER_ERROR;
      }
      strcpy(randomProp->propName,hold1);
      strcpy(randomProp->propDescr,type);
      insertBack((*obj)->properties,randomProp);
      free(hold1);
    }

    strcpy(lastLine,lineHold);

  }

  if(strncmp("END:VCALENDAR",currentLine,13)!=0)
  {
    deleteCalendar(*obj);
    *obj=NULL;
    free(currentLine);
    free(lineHold);
    free(lastLine);
    fclose(fp);
    return INV_CAL;
  }

  if(!versionCheck || !prodidCheck)
  {
    deleteCalendar(*obj);
    *obj=NULL;
    free(currentLine);
    free(lineHold);
    free(lastLine);
    fclose(fp);
    return INV_CAL;
  }

  if(getLength((*obj)->events)==0)
  {
    deleteCalendar(*obj);
    *obj=NULL;
    free(currentLine);
    free(lineHold);
    free(lastLine);
    fclose(fp);
    return INV_CAL;
  }

  free(currentLine);
  free(lineHold);
  free(lastLine);
  fclose(fp);
  return OK;

}

void deleteCalendar(Calendar*obj)
{
  if(obj==NULL)
  {
    return;
  }
  if(obj->events!=NULL)
  {
    freeList(obj->events);
  }
  if(obj->properties!=NULL)
  {
    freeList(obj->properties);
  }
  free(obj);
}

char *printCalendar(const Calendar *obj)
{
  if(obj==NULL)
  {
    return NULL;
  }
  char * toReturn=malloc(10000);
  if(toReturn==NULL)
  {
    return NULL;
  }
  char * version=malloc(11);
  if(toReturn==NULL)
  {
    free(toReturn);
    return NULL;
  }
  strcpy(toReturn,"VERSION:");
  sprintf(version,"%f",obj->version);
  strcat(toReturn,version);
  strcat(toReturn,"\n");
  strcat(toReturn,"PRODID:");
  strcat(toReturn,obj->prodID);
  strcat(toReturn,"\n");
  char *toStringThing=toString(obj->properties);
  strcat(toReturn,toStringThing);
  free(toStringThing);
  char *toStringThing2=toString(obj->events);
  strcat(toReturn,toStringThing2);
  strcat(toReturn,"END OF CALENDAR\n");
  free(toStringThing2);
  free(version);
  return toReturn;
}

void deleteEvent(void* toBeDeleted)
{
  if(toBeDeleted==NULL)
  {
    return;
  }
  Event *hi;
  hi=(Event*) toBeDeleted;
  freeList(hi->properties);
  freeList(hi->alarms);
  free(hi);
}

char* printEvent(void * toBePrinted)
{
  if(toBePrinted==NULL)
  {
    return NULL;
  }
  Event *hi;
  hi=(Event*) toBePrinted;
  char *toReturn=malloc(1000);
  if(toReturn==NULL)
  {
    return NULL;
  }
  char *dateHold;
  char *dateHold2;
  strcpy(toReturn,"EVENT:");
  strcat(toReturn,"\nUID:");
  strcat(toReturn,hi->UID);
  dateHold=printDate(&(hi->startDateTime));
  strcat(toReturn,dateHold);
  dateHold2=printDate(&(hi->creationDateTime));
  strcat(toReturn,dateHold2);
  char *toStringThing=toString(hi->properties);
  strcat(toReturn,toStringThing);
  char *toStringThing2=toString(hi->alarms);
  strcat(toReturn,toStringThing2);
  strcat(toReturn,"END OF EVENT");
  strcat(toReturn,"\n");
  free(toStringThing);
  free(toStringThing2);
  free(dateHold);
  free(dateHold2);
  return toReturn;
}

int compareEvents(const void* first, const void* second)
{
  if(first!=second)
  {
    return 1;
  }
  return 0;
}

void deleteProperty(void* toBeDeleted)
{
  if(toBeDeleted==NULL)
  {
    return;
  }
  free(toBeDeleted);
}

int compareProperties(const void* first, const void* second)
{
  if(first!=second)
  {
    return 1;
  }
  return 0;
}

char* printProperty(void* toBePrinted)
{
  if(toBePrinted==NULL)
  {
    return NULL;
  }
  Property *hi;
  hi=(Property*) toBePrinted;

  char *toReturn=malloc(strlen(hi->propName)+strlen(hi->propDescr)+4);
  if(toReturn==NULL)
  {
    return NULL;
  }
  strcpy(toReturn,hi->propName);
  strcat(toReturn,": ");
  strcat(toReturn,hi->propDescr);
  strcat(toReturn,"\n");
  return toReturn;
}

void deleteAlarm(void* toBeDeleted)
{
  if(toBeDeleted==NULL)
  {
    return;
  }
  Alarm *hi;
  hi=(Alarm*) toBeDeleted;
  free(hi->trigger);
  freeList(hi->properties);
  free(hi);
}

int compareAlarms(const void* first, const void* second)
{
  if(first==second)
  {
    return 0;
  }
  return 1;
}

char* printAlarm(void* toBePrinted)
{
  if(toBePrinted==NULL)
  {
    return NULL;
  }
  Alarm *hi;
  hi=(Alarm*) toBePrinted;
  char * toReturn=malloc(1000);
  if(toReturn==NULL)
  {
    return NULL;
  }
  strcpy(toReturn,"\nALARM STARTS:\n");
  strcat(toReturn,"ACTION: ");
  strcat(toReturn,hi->action);
  strcat(toReturn,"\nTRIGGER: ");
  strcat(toReturn,hi->trigger);
  strcat(toReturn,"\n");
  char *toStringThing=toString(hi->properties);
  strcat(toReturn,toStringThing);
  strcat(toReturn,"\nALARM ENDS\n");
  free(toStringThing);
  return toReturn;

}

void deleteDate(void* toBeDeleted)
{
  if(toBeDeleted!=NULL)
  {
    free(toBeDeleted);
  }
}

int compareDates(const void* first, const void* second)
{
  if(first==second)
  {
    return 0;
  }
  return 1;
}

char* printDate(void* toBePrinted)
{
  if(toBePrinted==NULL)
  {
    return NULL;
  }
  DateTime *hi;
  hi=(DateTime*) toBePrinted;
  char *toReturn=malloc(1000);
  if(toReturn==NULL)
  {
    return NULL;
  }
  strcpy(toReturn,"\nDATE:");
  strcat(toReturn,hi->date);
  strcat(toReturn,"\nTIME:");
  strcat(toReturn,hi->time);
  strcat(toReturn,"\n");
  if(hi->UTC)
  {
    strcat(toReturn,"UTC\n");
  }
  else
  {
    strcat(toReturn,"NON UTC\n");
  }
  return toReturn;
}

char* printError(ICalErrorCode err)
{
  char *toReturn=malloc(15);
  if(toReturn==NULL)
  {
    return NULL;
  }
  if(err==INV_FILE)
  {
    strcpy(toReturn,"INV_FILE");
  }
  else if(err==INV_CAL)
  {
    strcpy(toReturn,"INV_CAL");
  }
  else if(err==INV_VER)
  {
    strcpy(toReturn,"INV_VER");
  }
  else if(err==DUP_VER)
  {
    strcpy(toReturn,"DUP_VER");
  }
  else if(err==INV_PRODID)
  {
    strcpy(toReturn,"INV_PRODID");
  }
  else if(err==DUP_PRODID)
  {
    strcpy(toReturn,"DUP_PRODID");
  }
  else if(err==INV_EVENT)
  {
    strcpy(toReturn,"INV_EVENT");
  }
  else if(err==INV_DT)
  {
    strcpy(toReturn,"INV_DT");
  }
  else if(err==INV_ALARM)
  {
    strcpy(toReturn,"INV_ALARM");
  }
  else if(err==WRITE_ERROR)
  {
    strcpy(toReturn,"WRITE_ERROR");
  }
  else if(err==OTHER_ERROR)
  {
    strcpy(toReturn,"OTHER_ERROR");
  }
  else
  {
    strcpy(toReturn,"OK");
  }
  return toReturn;
}

ICalErrorCode writeCalendar(char* fileName,const Calendar* obj)
{
  if(obj==NULL)
  {
    return WRITE_ERROR;
  }
  if(fileName==NULL)
  {
    return WRITE_ERROR;
  }
  int length=strlen(fileName);
  if(fileName[length-4]!='.' || fileName[length-3]!='i' || fileName[length-2]!='c' || fileName[length-1]!='s')
  {
    return WRITE_ERROR;
  }

  FILE *fp;
  fp=fopen(fileName,"w");
  if(fp==NULL)
  {
    return WRITE_ERROR;
  }

  fprintf(fp,"BEGIN:VCALENDAR\r\n");
  fprintf(fp,"VERSION:%.1f\r\n",obj->version);
  fprintf(fp,"PRODID:%s\r\n",obj->prodID);
  ListIterator properties = createIterator(obj->properties);
  Property *propHolder=nextElement(&properties);

  while(propHolder!=NULL)
  {
    fprintf(fp,"%s:%s\r\n",propHolder->propName,propHolder->propDescr);
    propHolder=nextElement(&properties);
  }

  ListIterator events = createIterator(obj->events);
  Event *eventHolder=nextElement(&events);

  while(eventHolder!=NULL)
  {
    fprintf(fp,"BEGIN:VEVENT\r\n");
    fprintf(fp,"UID:%s\r\n",eventHolder->UID);

    fprintf(fp,"DTSTAMP:%sT%s",eventHolder->creationDateTime.date,eventHolder->creationDateTime.time);
    if(eventHolder->creationDateTime.UTC)fprintf(fp,"Z");
    fprintf(fp,"\r\n");

    fprintf(fp,"DTSTART:%sT%s",eventHolder->startDateTime.date,eventHolder->startDateTime.time);
    if(eventHolder->startDateTime.UTC)fprintf(fp,"Z");
    fprintf(fp,"\r\n");

    ListIterator properties2 = createIterator(eventHolder->properties);
    Property *propHolder2=nextElement(&properties2);

    while(propHolder2!=NULL)
    {
      fprintf(fp,"%s:%s\r\n",propHolder2->propName,propHolder2->propDescr);
      propHolder2=nextElement(&properties2);
    }

    ListIterator alarms = createIterator(eventHolder->alarms);
    Alarm *alarmHolder=nextElement(&alarms);

    while(alarmHolder!=NULL)
    {
      fprintf(fp,"BEGIN:VALARM\r\n");
      fprintf(fp,"ACTION:%s\r\n",alarmHolder->action);
      fprintf(fp,"TRIGGER:%s\r\n",alarmHolder->trigger);
      ListIterator properties3 = createIterator(alarmHolder->properties);
      Property *propHolder3=nextElement(&properties3);

      while(propHolder3!=NULL)
      {
        fprintf(fp,"%s:%s\r\n",propHolder3->propName,propHolder3->propDescr);
        propHolder3=nextElement(&properties3);
      }

      fprintf(fp,"END:VALARM\r\n");
      alarmHolder=nextElement(&alarms);
    }

    fprintf(fp,"END:VEVENT\r\n");
    eventHolder=nextElement(&events);
  }

  fprintf(fp,"END:VCALENDAR\r\n");
  fclose(fp);
  return OK;
}

ICalErrorCode validateCalendar(const Calendar* obj)
{
  if(obj==NULL)
  {
    return INV_CAL;
  }
  //First: Based on CalendarParser.h

  //PRODID CHECK
  if(strcmp(obj->prodID,"")==0 || strlen(obj->prodID)>998)
  {
    return INV_CAL;
  }

  //Checking lists for NULL
  if(obj->events==NULL || obj->properties==NULL)
  {
    return INV_CAL;
  }

  //Check size of events
  if(getLength(obj->events)==0)
  {
    return INV_CAL;
  }

  //Checks all properties
  ListIterator properties = createIterator(obj->properties);
  Property *propHolder=nextElement(&properties);
  bool method=false,calscale=false;
  while(propHolder!=NULL)
  {
    if(strcmp(propHolder->propName,"CALSCALE")!=0 && strcmp(propHolder->propName,"METHOD")!=0)
    {
      return INV_CAL;
    }
    if(strcmp("",propHolder->propDescr)==0 || propHolder->propDescr==NULL)
    {
      return INV_CAL;
    }
    if(strcmp(propHolder->propName,"METHOD")==0)
    {
      if(method)
      {
        return INV_CAL;
      }
      method=true;
    }
    if(strcmp(propHolder->propName,"CALSCALE")==0)
    {
      if(calscale)
      {
        return INV_CAL;
      }
      calscale=true;
    }
    ListIterator properties2 = createIterator(obj->properties);
    Property *propHolder2=nextElement(&properties2);

    while(propHolder2!=NULL)
    {
      //Kinda shitty test for duplicate properties
      if(strcmp(propHolder2->propName,propHolder->propName)==0 && strcmp(propHolder2->propDescr,propHolder->propDescr)!=0)
      {
        return INV_CAL;
      }
      propHolder2=nextElement(&properties2);
    }

    propHolder=nextElement(&properties);
  }

  ListIterator events = createIterator(obj->events);
  Event *eventHolder=nextElement(&events);

  while(eventHolder!=NULL)
  {
    //UID Checker
    if(strcmp(eventHolder->UID,"")==0 || strlen(eventHolder->UID)>199)
    {
      return INV_EVENT;
    }
    //Check null lists
    if(eventHolder->properties==NULL || eventHolder->alarms==NULL)
    {
      return INV_EVENT;
    }
    //Dates too long
    if(strlen(eventHolder->creationDateTime.date)>8 || strlen(eventHolder->creationDateTime.time)>6 || strlen(eventHolder->startDateTime.date)>8 || strlen(eventHolder->startDateTime.time)>6)
    {
      return INV_EVENT;
    }
    //Dates Empty
    if(strlen(eventHolder->creationDateTime.date)==0 || strlen(eventHolder->creationDateTime.time)==0 || strlen(eventHolder->startDateTime.date)==0 || strlen(eventHolder->startDateTime.time)==0)
    {
      return INV_EVENT;
    }

    //Check properties of eventToJSON
    ListIterator eventProperties = createIterator(eventHolder->properties);
    Property *evPropHold=nextElement(&eventProperties);
    bool class=false,created=false,description=false,geo=false,lastmod=false,location=false,organizer=false,priority=false,seq=false,status=false,summary=false,transp=false,url=false,recurid=false,dateone=false;
    while(evPropHold!=NULL)
    {
      if(strcmp("",evPropHold->propDescr)==0)
      {
        return INV_EVENT;
      }
      if(strcmp(evPropHold->propName,"CLASS")!=0 && strcmp(evPropHold->propName,"CREATED")!=0 && strcmp(evPropHold->propName,"DESCRIPTION")!=0 && strcmp(evPropHold->propName,"GEO")!=0 && strcmp(evPropHold->propName,"LAST-MODIFIED")!=0 && strcmp(evPropHold->propName,"LOCATION")!=0 && strcmp(evPropHold->propName,"ORGANIZER")!=0 && strcmp(evPropHold->propName,"PRIORITY")!=0 && strcmp(evPropHold->propName,"SEQUENCE")!=0 && strcmp(evPropHold->propName,"STATUS")!=0 && strcmp(evPropHold->propName,"SUMMARY")!=0 && strcmp(evPropHold->propName,"TRANSP")!=0 && strcmp(evPropHold->propName,"URL")!=0 && strcmp(evPropHold->propName,"RECURRENCE-ID")!=0 && strcmp(evPropHold->propName,"RRULE")!=0 && strcmp(evPropHold->propName,"DTEND")!=0 && strcmp(evPropHold->propName,"DURATION")!=0 && strcmp(evPropHold->propName,"ATTACH")!=0 && strcmp(evPropHold->propName,"ATTENDEE")!=0 && strcmp(evPropHold->propName,"CATEGORIES")!=0 && strcmp(evPropHold->propName,"COMMENT")!=0 && strcmp(evPropHold->propName,"CONTACT")!=0 && strcmp(evPropHold->propName,"EXDATE")!=0 && strcmp(evPropHold->propName,"REQUEST-STATUS")!=0 && strcmp(evPropHold->propName,"RELATED-TO")!=0 && strcmp(evPropHold->propName,"RESOURCES")!=0 && strcmp(evPropHold->propName,"RDATE")!=0 )
      {
        return INV_EVENT;
      }
      if(strcmp(evPropHold->propName,"CLASS")==0)
      {
        if(class)
        {
          return INV_EVENT;
        }
        class=true;
      }
      if(strcmp(evPropHold->propName,"CREATED")==0)
      {
        if(created)
        {
          return INV_EVENT;
        }
        created=true;
      }
      if(strcmp(evPropHold->propName,"DESCRIPTION")==0)
      {
        if(description)
        {
          return INV_EVENT;
        }
        description=true;
      }
      if(strcmp(evPropHold->propName,"GEO")==0)
      {
        if(geo)
        {
          return INV_EVENT;
        }
        geo=true;
      }
      if(strcmp(evPropHold->propName,"LAST-MODIFIED")==0)
      {
        if(lastmod)
        {
          return INV_EVENT;
        }
        lastmod=true;
      }
      if(strcmp(evPropHold->propName,"LOCATION")==0)
      {
        if(location)
        {
          return INV_EVENT;
        }
        location=true;
      }
      if(strcmp(evPropHold->propName,"ORGANIZER")==0)
      {
        if(organizer)
        {
          return INV_EVENT;
        }
        organizer=true;
      }
      if(strcmp(evPropHold->propName,"PRIORITY")==0)
      {
        if(priority)
        {
          return INV_EVENT;
        }
        priority=true;
      }
      if(strcmp(evPropHold->propName,"SEQUENCE")==0)
      {
        if(seq)
        {
          return INV_EVENT;
        }
        seq=true;
      }
      if(strcmp(evPropHold->propName,"STATUS")==0)
      {
        if(status)
        {
          return INV_EVENT;
        }
        status=true;
      }
      if(strcmp(evPropHold->propName,"SUMMARY")==0)
      {
        if(summary)
        {
          return INV_EVENT;
        }
        summary=true;
      }
      if(strcmp(evPropHold->propName,"TRANSP")==0)
      {
        if(transp)
        {
          return INV_EVENT;
        }
        transp=true;
      }
      if(strcmp(evPropHold->propName,"URL")==0)
      {
        if(url)
        {
          return INV_EVENT;
        }
        url=true;
      }
      if(strcmp(evPropHold->propName,"RECURRENCE-ID")==0)
      {
        if(recurid)
        {
          return INV_EVENT;
        }
        recurid=true;
      }
      if(strcmp(evPropHold->propName,"DTEND")==0 || strcmp(evPropHold->propName,"DURATION")==0)
      {
        if(dateone)
        {
          return INV_EVENT;
        }
        dateone=true;
      }


      evPropHold=nextElement(&eventProperties);
    }
    eventHolder=nextElement(&events);
  }

  events = createIterator(obj->events);
  eventHolder=nextElement(&events);

  while(eventHolder!=NULL)
  {
    ListIterator eventAlarms = createIterator(eventHolder->alarms);
    Alarm *eventAlarm=nextElement(&eventAlarms);

    while(eventAlarm!=NULL)
    {
      if(eventAlarm->properties==NULL)
      {
        return INV_ALARM;
      }
      if(strcmp(eventAlarm->action,"AUDIO")!=0)
      {
        return INV_ALARM;
      }
      if(eventAlarm->trigger==NULL || strcmp(eventAlarm->trigger,"")==0)
      {
        return INV_ALARM;
      }

      properties = createIterator(eventAlarm->properties);
      propHolder = nextElement(&properties);
      bool duration=false,repeat=false,attach=false;
      while(propHolder!=NULL)
      {
        if(strcmp("",propHolder->propDescr)==0)
        {
          return INV_ALARM;
        }
        if(strcmp(propHolder->propName,"DURATION")!=0 && strcmp(propHolder->propName,"REPEAT")!=0 && strcmp(propHolder->propName,"ATTACH")!=0)
        {
          return INV_ALARM;
        }
        if(strcmp(propHolder->propName,"DURATION")==0)
        {
          if(duration)
          {
            return INV_ALARM;
          }
          duration=true;
        }
        if(strcmp(propHolder->propName,"REPEAT")==0)
        {
          if(repeat)
          {
            return INV_ALARM;
          }
          repeat=true;
        }
        if(strcmp(propHolder->propName,"ATTACH")==0)
        {
          if(attach)
          {
            return INV_ALARM;
          }
          attach=true;
        }


        propHolder=nextElement(&properties);
      }
      if(duration!=repeat)
      {
        return INV_ALARM;
      }

      eventAlarm=nextElement(&eventAlarms);
    }



    eventHolder=nextElement(&events);
  }

  return OK;
}

char* dtToJSON(DateTime prop)
{
  char *toReturn=malloc(100);
  strcpy(toReturn,"{\"date\":\"");
  strcat(toReturn,prop.date);
  strcat(toReturn,"\",\"time\":\"");
  strcat(toReturn,prop.time);
  strcat(toReturn,"\",\"isUTC\":");
  if(prop.UTC)
  {
    strcat(toReturn,"true");
  }
  else
  {
    strcat(toReturn,"false");
  }
  strcat(toReturn,"}");
  return toReturn;
}

char *eventToJSON(const Event* event)
{
  char* mightReturn=malloc(3);
  if(event==NULL)
  {
    strcpy(mightReturn,"{}");
    return mightReturn;
  }
  free(mightReturn);
  char *toReturn=malloc(300);
  strcpy(toReturn,"{\"startDT\":");
  char *dateHold=dtToJSON(event->startDateTime);
  strcat(toReturn,dateHold);
  free(dateHold);
  strcat(toReturn,",\"numProps\":");

  char numProps[50];
  sprintf(numProps,"%d",getLength(event->properties)+3);
  strcat(toReturn,numProps);

  strcat(toReturn,",\"numAlarms\":");
  char numAlarms[50];
  sprintf(numAlarms,"%d",getLength(event->alarms));
  strcat(toReturn,numAlarms);

  strcat(toReturn,",\"summary\":\"");
  ListIterator itr = createIterator(event->properties);

  Property* data = nextElement(&itr);
  while (data != NULL)
  {
    if (strcmp(data->propName,"SUMMARY")==0)
    {
      break;
    }
    data = nextElement(&itr);
  }
  if(data!=NULL)
  {
    strcat(toReturn,data->propDescr);
  }
  strcat(toReturn,"\"}");
  return toReturn;
}

char* eventListToJSON(const List* eventList)
{
  char* mightReturn=malloc(3);
  if(eventList==NULL)
  {
    strcpy(mightReturn,"[]");
    return mightReturn;
  }
  free(mightReturn);

  char* toReturn=malloc(10000);
  char* holder;
  strcpy(toReturn,"[");
  ListIterator itr = createIterator((List *)eventList);
  Event *eventHolder = nextElement(&itr);
  while(eventHolder!=NULL)
  {
    holder=eventToJSON(eventHolder);
    strcat(toReturn,holder);
    free(holder);
    eventHolder = nextElement(&itr);
    if(eventHolder!=NULL)
    {
      strcat(toReturn,",");
    }
  }
  strcat(toReturn,"]");
  return toReturn;
}

char* calendarToJSON(const Calendar* cal)
{
  char* mightReturn=malloc(3);
  if(cal==NULL)
  {
    strcpy(mightReturn,"{}");
    return mightReturn;
  }
  free(mightReturn);

  char *toReturn=malloc(200);
  strcpy(toReturn,"{\"version\":");
  int verHold=(int)cal->version;

  char versionNum[50];
  sprintf(versionNum,"%d",verHold);
  strcat(toReturn,versionNum);
  strcat(toReturn,",\"prodID\":\"");
  strcat(toReturn,cal->prodID);
  strcat(toReturn,"\",\"numProps\":");

  char numProps[50];
  sprintf(numProps,"%d",getLength(cal->properties)+2);
  strcat(toReturn,numProps);

  strcat(toReturn,",\"numEvents\":");

  char numEvents[50];
  sprintf(numEvents,"%d",getLength(cal->events));
  strcat(toReturn,numEvents);

  strcat(toReturn,"}");
  return toReturn;
}

Calendar* JSONtoCalendar(const char* str)
{
  if(str==NULL)
  {
    return NULL;
  }
  Calendar *toReturn=malloc(sizeof(Calendar));
  toReturn->properties=initializeList(printProperty,deleteProperty,compareProperties);
  toReturn->events=initializeList(printEvent,deleteEvent,compareEvents);
  char version[50];
  char current;
  int count=0,count2=11;

  current=str[count2];
  while(current!=',')
  {
    version[count]=current;
    current=str[++count2];
    count++;
  }

  toReturn->version=atoi(version);

  count2=12;

  current=str[count2];
  while(current!=':')
  {
    current=str[++count2];
  }
  char prodID[1000];
  count2+=2;
  count=0;
  current=str[count2];
  while(current!='"')
  {
    prodID[count]=current;
    current=str[++count2];
    count++;
  }
  prodID[count]='\0';
  strcpy(toReturn->prodID,prodID);
  return toReturn;

}

Event* JSONtoEvent(const char* str)
{
  if(str==NULL)
  {
    return NULL;
  }
  Event * toReturn=malloc(sizeof(Event));
  toReturn->properties=initializeList(printProperty,deleteProperty,compareProperties);
  toReturn->alarms=initializeList(printAlarm,deleteAlarm,compareAlarms);

  char uid[1000];
  char current;
  int count=0,count2=8;

  current=str[count2];
  while(current!='"')
  {
    uid[count]=current;
    current=str[++count2];
    count++;
  }
  uid[count]='\0';
  strcpy(toReturn->UID,uid);
  return toReturn;
}

void addEvent(Calendar* cal, Event* toBeAdded)
{
  if(cal==NULL || toBeAdded==NULL || cal->events==NULL)
  {
    return;
  }
  insertBack(cal->events,toBeAdded);

}












//dsds
