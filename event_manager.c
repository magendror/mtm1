#include "event_manager.h"
#include "date.h"
#include "priority_queue.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#define EM_IS_NULL -1

//!!! Turn into pq inwhich the data is member ang the priority is number !!!

typedef struct Member_t{
    int id;
    char* name;
    int num_of_events;
    struct Member_t* next;
}*Member;

typedef struct Event_t{
    Member first_member;
    Member member_iterator;
    int event_id;
    char* event_name;
    Date date;
}*Event;
struct EventManager_t{
    PriorityQueue event_list;
    Event first_event;// 
    Event iterator;
    Member member_iterator;
    Member first_member;
    Date start_date;
    Date current_date;
};

static Event copyEvent(Event event){
    Event new_event=malloc(sizeof(Event));
    if(new_event==NULL){
        return NULL;
    }
    Member new_member=malloc(sizeof(Member));
    if(new_member==NULL){
        free(new_event);
        return NULL;
    }
    char* new_event_name=malloc(sizeof(char)*(strlen(event->event_name)+1));
    if(new_event_name==NULL){
        free(new_member);
        free(new_event);
        return NULL;
    }
    Date new_date=malloc(sizeof(Date));
    if(new_event_name==NULL){
        free(new_member);
        free(new_event);
        free(new_event_name);
        return NULL;
    }
    Member new_member=malloc(sizeof(Member));
    if(new_event_name==NULL){
        free(new_member);
        free(new_event);
        free(new_event_name);
        return NULL;
    }
    Date new_date=dateCopy(event->date);
    strcpy(new_event_name,event->event_name);
    new_event->event_name=new_event_name;
    new_event->event_id=event->event_id;
    new_event->first_member=new_member;
    new_event->date=new_date;
    return new_event;
}

static void freeEvent(Event event){
    free(event->event_name);
    dateDestroy(event->date);
    event->member_iterator=event->first_member;
    Member local_iterator;
    while(event->member_iterator!=NULL){
        local_iterator=event->member_iterator->next;
        free(event->member_iterator->name);
        free(event->member_iterator);
        event->member_iterator=local_iterator;
    }
    free(event);
}

static bool equalEvent(Event event1,Event event2){
    if(event1->event_id!=event2->event_id){
        return false;
    }
    if(strcmp(event1->event_name,event2->event_name)!=0){
        return false;
    }
    return true; 
}

static int dateCompareReversed(Date date1,Date date2){
    return -1*dateCompare(date1,date2);
}

EventManager createEventManager(Date date){
    EventManager event_manager=malloc(sizeof(EventManager));
    if (event_manager==NULL){
        return NULL;
    }
    event_manager->event_list=pqCreate(copyEvent,freeEvent,equalEvent,
                                dateCopy,dateDestroy,dateCompareReversed);
    event_manager->first_member=NULL;
    event_manager->member_iterator=NULL;
    event_manager->start_date=NULL;
    event_manager->current_date=NULL;
    return event_manager;
}
/*
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
*/
void destroyEventManager(EventManager em){
    if(em==NULL){
        return;
    }
    pqDestroy(em->event_list);
    Member local_member_iterator;
    em->member_iterator=em->first_member;
    while(em->member_iterator!=NULL){
        local_member_iterator=em->member_iterator->next;
        free(em->member_iterator->name);
        free(em->member_iterator);
        em->member_iterator=local_member_iterator;
    }
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
    PQ_FOREACH(Event,current_event,em->event_list){
        if((dateCompare(current_event->date,date)==0)&&(strcmp(current_event->event_name,event_name)==0)){
            EM_EVENT_ALREADY_EXISTS;
        }
    }
    //
    //id not taken check
    PQ_FOREACH(Event,current_event,em->event_list){
        if(current_event->event_id==event_id){
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
    new_event->event_id=event_id;
    new_event->event_name=event_name;
    new_event->first_member=NULL;
    new_event->member_iterator=NULL;
    if(pqInsert(em->event_list,new_event,date)==PQ_OUT_OF_MEMORY){
        free(new_event);
        destroyEventManager(em);
        exit;
    }
    free(new_event);
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
    dateDestroy(new_date);
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
    PQ_FOREACH(Event,current_event,em->event_list){
        if (current_event->event_id==event_id){
            current_event->date=new_date;
            pqChangePriority(em->event_list,current_event,current_event->date,new_date);
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
    if(em==NULL){
        return EM_NULL_ARGUMENT;
    }
    if(days<0){
        return EM_INVALID_DATE;
    }
    for (int i=0; i<days;i++){
        dateTick(em->current_date);
    }
    while((dateCompare(((Event)pqGetFirst(em->event_list))->date,em->current_date)<0)&&(pqGetSize(em->event_list)>0)){
            pqRemove(em->event_list);
        }
    return EM_SUCCESS;
}

char* emGetNextEvent(EventManager em){
    if(em==NULL){
        return NULL;
    }
    return ((Event)pqGetFirst(em->event_list))->event_name;
}

void emPrintAllEvents(EventManager em, const char* file_name){
    FILE* write_to_file=fopen(file_name,"w");
    if(write_to_file==NULL){
        return;
    }
    PQ_FOREACH(Event,current_event,em->event_list){
        int day,month,year;
        dateGet(current_event->date,&day,&month,&year);
        fprintf("%s,%d.%d.%d",current_event->event_name,day,month,year);
        current_event->member_iterator=current_event->first_member;
        while (current_event->member_iterator!=NULL){
            fprintf(",%s",current_event->member_iterator->name);
        }
        fprintf("\n",current_event->member_iterator->name);
    }
    fclose(write_to_file);
}
