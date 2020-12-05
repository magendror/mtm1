#include "event_manager.h"
#include "date.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#define EM_IS_NULL -1

typedef struct Member_t{
    int id;
    char* name;
    struct Event_t* member_first_event;
    struct Member_t* next;
}*Member;

typedef struct Event_t{
    Date date;
    Member first_member;
    int event_id;
    char* event_name;
    struct Event_t* next; 
}*Event;
struct EventManager_t{
    Event event; // change one of them 
    Event first_event;// 
    Event iterator;
    Member member_iterator;
    Member first_member;
    Date start_date;
    Date current_date;
};

EventManager createEventManager(Date date){
    EventManager event_manager=malloc(sizeof(EventManager));
    if (event_manager==NULL){
        return NULL;
    }
    event_manager->event=NULL;
    event_manager->first_event=NULL;
    event_manager->first_member=NULL;
    event_manager->iterator=NULL;
    event_manager->member_iterator=NULL;
    event_manager->start_date=NULL;
    event_manager->current_date=NULL;
    return event_manager;
}

static void freeEventsAll(EventManager em,Event event){
    em->member_iterator=event->first_member;
    Member local_iterator;
    while(em->member_iterator!=NULL){
        local_iterator=em->member_iterator->next;
        free(em->member_iterator->name);
        free(em->member_iterator);
        em->member_iterator=local_iterator;
    }
    Event local_event_iterator;
    em->iterator=em->first_event;
    while(em->iterator!=NULL){
        local_event_iterator=em->iterator->next;
        free(em->iterator->event_name);
        free(em->iterator);
        em->iterator=local_event_iterator;
    }
}

static void freeMemberAll(EventManager em,Member member){
    em->iterator=member->member_first_event;
    Event local_iterator;
    while(em->iterator!=NULL){
        local_iterator=em->iterator->next;
        free(em->iterator->event_name);
        free(em->iterator);
        em->iterator=local_iterator;
    }
    Member local_member_iterator;
    em->member_iterator=em->first_member;
    while(em->member_iterator!=NULL){
        local_member_iterator=em->member_iterator->next;
        free(em->member_iterator->name);
        free(em->member_iterator);
        em->member_iterator=local_member_iterator;
    }
}
void destroyEventManager(EventManager em){
    if(em==NULL){
        return;
    }
    freeEventsAll(em, em->first_event);
    freeMemberAll(em,em->first_member);
    free(em);
}

EventManagerResult validDataCheck(EventManager em, char* event_name, Date date, int event_id){
    //null argument check // ask what is about
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
    Date new_date=dateCopy(date);
    new_event->date=new_date;
    char* new_event_name=malloc(sizeof(char)*(strlen(event_name)+1));
    strcpy(new_event_name,event_name);
    new_event->event_name=new_event_name;
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
    if(em==NULL||new_date==NULL){
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
            Date new_date_copy = dateCopy(new_date);
            dateDestroy(em->iterator->date);
            em->iterator->date=new_date_copy;
            return EM_SUCCESS;
        }
    }
    return EM_EVENT_ID_NOT_EXISTS;
}

static Member creatNewMember(char* member_name, int member_id){
    Member new_member=malloc(sizeof(Member));
    if(new_member==NULL){
        return NULL;
    }
    new_member->name=malloc(sizeof(char)*(strlen(member_name)+1));
    if(new_member->name==NULL){
        return NULL;
    }
    new_member->name=member_name;
    new_member->id=member_id;
    new_member->member_first_event=NULL;
    new_member->next=NULL;
    return new_member;
}

EventManagerResult emAddMember(EventManager em, char* member_name, int member_id){
    if(em==NULL||member_name==NULL){
        return EM_NULL_ARGUMENT;
    }
    if(member_id<0){
        return EM_INVALID_MEMBER_ID;
    }
    Member new_member=creatNewMember(member_name,member_id);
    if(new_member==NULL){
        return EM_OUT_OF_MEMORY;
    }
    if(em->first_member==NULL){
        em->first_member=new_member;
    }
    em->member_iterator=em->first_member;
    while(em->member_iterator){
        if(member_id==em->member_iterator->id){
            free(new_member);
            return EM_MEMBER_ID_ALREADY_EXISTS;
        }
        em->member_iterator=em->member_iterator->next;
    }
    new_member->next=em->first_member;
    em->first_member=new_member;
    return EM_SUCCESS;
}


static Member findMember(EventManager em, int member_id){
    em->member_iterator=em->first_member;
    while(em->member_iterator){
        if(em->member_iterator->id==member_id){
            return em->member_iterator;
        }
        em->member_iterator=em->member_iterator->next;
    }
    return NULL;
}

static Event findEvent(EventManager em, int event_id){
    em->iterator=em->first_event;
    while(em->iterator){
        if(em->iterator->event_id==event_id){
            return em->iterator;
        }
        em->iterator=em->iterator->next;
    }
    return NULL;
}

static bool connectEventToMember(Event event,Member member){
    Member new_member=malloc(sizeof(Member));
    if(new_member==NULL){
       return false;
    }
    new_member->name=malloc(sizeof(char)*(strlen(member->name)+1));
    if(new_member->name==NULL){
        return NULL;
    }
    new_member=member;
    new_member->next=event->first_member;
    event->first_member=new_member;
    return true;
}

static bool connectMemberToEvent(Member member,Event event){
    Event new_event=malloc(sizeof(Event));
    if(new_event==NULL){
       return false;
    }
    new_event->event_name=malloc(sizeof(char)*(strlen(event->event_name)+1));
    if(new_event->event_name==NULL){
        return false;
    }
    new_event=event;
    new_event->next=member->member_first_event;
    member->member_first_event=new_event;
    return true;
}

static bool memberDuplication(EventManager em,int member_id,Event event){
    em->member_iterator=event->first_member;
    while(em->member_iterator){
        if(em->member_iterator->id==member_id){
            return true;
        }
    }
    return false;
}
EventManagerResult emAddMemberToEvent(EventManager em, int member_id, int event_id){
    if(em==NULL){
        return EM_NULL_ARGUMENT;
    }
    if(event_id<0){
        return EM_INVALID_EVENT_ID;
    }
    if(member_id<0){
        return EM_INVALID_MEMBER_ID;
    }
    Event event=findEvent(em,event_id);
    if(event==NULL){
        return EM_EVENT_ID_NOT_EXISTS;
    }
    Member member=findMember(em,member_id);
    if(member==NULL){
        return EM_MEMBER_ID_NOT_EXISTS;
    }
    if(memberDuplication(em,member_id,event)){
        return EM_EVENT_AND_MEMBER_ALREADY_LINKED;
    }
    if(!connectEventToMember(event,member)){
        return EM_OUT_OF_MEMORY;
    }
    if(!connectMemberToEvent(member,event)){
        return EM_OUT_OF_MEMORY;
    }
    return EM_SUCCESS;
}

static bool removeMemberFromEvent(EventManager em,Event event,int member_id){
    em->member_iterator=event->first_member;
    Member last_member=NULL;
    while(em->member_iterator){
        if(em->member_iterator->id==member_id){
            if(last_member==NULL){
                event->first_member=em->member_iterator->next;
            }
            else{
                last_member->next=em->member_iterator->next;
            }
            free(em->member_iterator);
                return true;

        }
        em->member_iterator=em->member_iterator->next;
    }
    return false;
}
EventManagerResult emRemoveMemberFromEvent (EventManager em, int member_id, int event_id){
    if(em==NULL){
        return EM_NULL_ARGUMENT;
    }
    if(event_id<0){
        return EM_INVALID_EVENT_ID;
    }
    if(member_id<0){
        return EM_INVALID_MEMBER_ID;
    }
    Event event=findEvent(em,event_id);
    if(event==NULL){
        return EM_EVENT_ID_NOT_EXISTS;
    }
    Member member=findMember(em,member_id);
    if(member==NULL){
        return EM_MEMBER_ID_NOT_EXISTS;
    }
    if(removeMemberFromEvent(em,event,member_id)){
        return EM_SUCCESS;
    }
    return EM_EVENT_AND_MEMBER_NOT_LINKED;
}

int emGetEventsAmount(EventManager em){
    if(em==NULL){
        return EM_IS_NULL;
    }
    int events_amount=0;
    em->iterator=em->first_event;
    while(em->iterator){
        events_amount++;
        em->iterator=em->iterator->next;
    }
    return events_amount;
}

EventManagerResult emTick(EventManager em, int days){
    for (int i=0; i<days;i++){
        dateTick(em->current_date);
    }
    while((dateCompare(em->first_event->date,em->current_date)<0)&&(em->first_event!=NULL)){
        emRemoveEvent(em,em->first_event->event_id);
    }
    em->iterator=em->first_event;
    while(em->iterator==NULL){
        if(dateCompare(em->first_event->date,em->current_date)<0){
            emRemoveEvent(em,em->iterator->event_id);
        }
    }
}