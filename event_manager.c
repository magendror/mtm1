#include "event_manager.h"
#include "date.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

typedef struct Member_t{
    int id;
    char* name;
    struct Member_t* next;
}*Member;

typedef struct Event_t{
    Date date;
    Member member;
    int event_id;
    char* event_name;
    struct Event_t* next; 
}*Event;
struct EventManager_t{
    Event event;
    Event iterator;
    Member member;
    Date start_date;
    Date current_date;
};

EventManager createEventManager(Date date){
    EventManager event_manager=malloc(sizeof(EventManager));
    if (event_manager==NULL){
        return NULL;
    }
    event_manager->event=NULL;
    event_manager->member=NULL;
    return event_manager;
}

void destroyEventManager(EventManager em){
    free(em->event->date);
    free(em->event->member);
    free(em->member);
    free(em);
}

EventManagerResult validDataCheck(EventManager em, char* event_name, Date date, int event_id){
    //null argument check
    if(em==NULL||event_name==NULL||event_id==NULL||date==NULL){
        return EM_NULL_ARGUMENT;
    }
    //
    //date check
    if(dateCompare(date,em->current_date)<=0){
        return EM_INVALID_DATE;
    }
    //
    //id positive check
    if(event_id<0){
        return EM_INVALID_EVENT_ID;
    }
    //
    //name&check check
    em->iterator=em->event;
    while(em->iterator!=NULL){
        if((dateCompare(em->iterator->date,date)==0)&&(strcmp(em->iterator->event_name,event_name)==0)){
            EM_EVENT_ALREADY_EXISTS;
        }
    }
    //
    //id not taken check
    em->iterator=em->event;
    while(em->iterator!=NULL){
        if(em->iterator->event_id==event_id){
            return EM_EVENT_ID_ALREADY_EXISTS;
        }
    }
    //
    return EM_SUCCESS;
}


EventManagerResult emAddEventByDate(EventManager em, char* event_name, Date date, int event_id){
    EventManagerResult result = validDataCheck(em,event_name,date,event_id);
    if(result!=EM_SUCCESS){
        return result;
    }
    Event new_event = malloc(sizeof(Event));
    if (new_event==NULL){
        return EM_OUT_OF_MEMORY;
    }
    new_event->date=date;
    new_event->event_name=malloc(sizeof(char)*(strlen(event_name)+1));
    new_event->event_id=event_id;
    new_event->next=NULL;
    em->iterator=em->event;
    if(em->event!=NULL){
        while(em->iterator->next!=NULL){
            em->iterator=em->iterator->next;
        }
    }
    em->iterator->next=new_event;
    return EM_SUCCESS;
}

EventManagerResult emAddEventByDiff(EventManager em, char* event_name, int days, int event_id){
    Date new_date =dateCopy(em->current_date);
    for(int i=0;i<days;i++){
        dateTick(new_date);
    }
    EventManagerResult result = emAddEventByDate(em,event_name,new_date,event_id);
    if (result!=EM_SUCCESS){
        dateDestroy(new_date);
        return result;
    }
    return EM_SUCCESS;
}

EventManagerResult emChangeEventDate(EventManager em, int event_id, Date new_date){
    if(em==NULL||event_id==NULL||new_date==NULL){
        return EM_NULL_ARGUMENT;
    }
    if(dateCompare(new_date,em->current_date)<=0){
        return EM_INVALID_DATE;
    }
    if(event_id<0){
        return EM_INVALID_EVENT_ID;
    }
    em->iterator=em->event;
    while(em->iterator!=NULL){
        if (em->iterator->event_id==event_id){
            if(dateCompare(em->iterator->date,new_date)==0){
                return EM_EVENT_ALREADY_EXISTS;
            }
            em->iterator->date=new_date;
            return EM_SUCCESS;
        }
    }
    return EM_EVENT_ID_NOT_EXISTS;
}


