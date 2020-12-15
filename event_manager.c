#include "event_manager.h"
#include "date.h"
#include "priority_queue.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#define EM_IS_NULL -1
#define POSITIVE_NUM 1
#define NEGATIVE_NUM -1 


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
    PriorityQueue member_list;
    Date start_date;
    Date current_date;
};


static void free_member(PQElement member){
    free(((Member)member)->name);
    free(member);
}

static PQElement copy_member(PQElement member){
    Member new_member=malloc(sizeof(struct Member_t));
    if(new_member==NULL){
        return NULL;
    }
    char* new_member_name=malloc(sizeof(char)*(strlen(((Member)member)->name)+1));
    if(new_member_name==NULL){
        free(new_member);
        return NULL;
    }

    strcpy(new_member_name,((Member)member)->name);
    new_member->name=new_member_name;
    new_member->num_of_events=((Member)member)->num_of_events;
    new_member->id=((Member)member)->id;
    new_member->next=NULL;
    return new_member;
}

static PQElement copyEvent(PQElement event){
    Event new_event=malloc(sizeof(struct Event_t));
    if(new_event==NULL){
        return NULL;
    }
    char* new_event_name=malloc(sizeof(char)*(strlen(((Event)event)->event_name)+1));
    if(new_event_name==NULL){
        free(new_event);
        return NULL;
    }
    Member new_member_iterator=NULL;
    Date new_date=dateCopy(((Event)event)->date);
    strcpy(new_event_name,((Event)event)->event_name);
    new_event->event_name=new_event_name;
    new_event->event_id=((Event)event)->event_id;
    new_event->date=new_date; //
    new_event->first_member=NULL;
    ((Event)event)->member_iterator=((Event)event)->first_member;
    while(((Event)event)->member_iterator!=NULL){
        Member new_member=copy_member(((Event)event)->member_iterator);
        if(new_member==NULL){
            return NULL;
        }
        if(new_event->first_member==NULL){//this is the first member
            new_event->first_member=new_member;  
            new_member_iterator=new_event->first_member;
        }
        else{
            new_member_iterator->next=new_member;
            new_member_iterator=new_member_iterator->next;
        }
        ((Event)event)->member_iterator=((Event)event)->member_iterator->next; 
    }
    return new_event;
}

static void freeEvent(PQElement event){
    free(((Event)event)->event_name);
    dateDestroy(((Event)event)->date);
    ((Event)event)->member_iterator=((Event)event)->first_member;
    Member local_iterator;
    while(((Event)event)->member_iterator!=NULL){
        local_iterator=((Event)event)->member_iterator;
        ((Event)event)->member_iterator=((Event)event)->member_iterator->next;
        free_member(local_iterator);
    }
    free(event);
}

static bool equalEventGeneric(PQElement event1,PQElement event2){
    if((((Event)event1)->event_id==((Event)event2)->event_id)&&
    (strcmp(((Event)event1)->event_name,((Event)event2)->event_name)==0)){
        return true;
    }
    return false;
}

static int dateCompareReversed(PQElementPriority date1,PQElementPriority date2){
    return -1*dateCompare((Date)date1,(Date)date2);
}

static PQElementPriority dateCopyGeneric(PQElementPriority date1){
    return dateCopy((Date)date1);
}

static void dateFree(PQElementPriority date1){
    dateDestroy((Date)date1);
}

void destroyEventManager(EventManager em){
    if(em==NULL){
        return;
    }
    pqDestroy(em->event_list);
    pqDestroy(em->member_list);
    dateDestroy(em->current_date);
    dateDestroy(em->start_date);
    free(em);
}


static bool equal_member(PQElement member1,PQElement member2){
    bool equal_id=(((Member)member1)->id==((Member)member2)->id);
    bool equal_name=(strcmp(((Member)member1)->name,((Member)member2)->name)==0);
    return ((equal_id)&&(equal_name));
}

static int compareMemberNumOfEvents(PQElementPriority member1,PQElementPriority member2){
    if(((Member)member1)->num_of_events>((Member)member2)->num_of_events){
        return POSITIVE_NUM;
    }
    else if (((Member)member1)->num_of_events<((Member)member2)->num_of_events){
        return NEGATIVE_NUM;
    }
    else{
        return ((Member)member2)->id-((Member)member1)->id;
    }
}

EventManager createEventManager(Date date){
    if(date==NULL){
        return NULL;
    }
    EventManager event_manager=malloc(sizeof(struct EventManager_t));
    if (event_manager==NULL){
        return NULL;
    }
    event_manager->event_list=pqCreate(copyEvent,freeEvent,equalEventGeneric,
                                        dateCopyGeneric,dateFree,dateCompareReversed);
    event_manager->member_list=pqCreate(copy_member,free_member,equal_member
                                        ,copy_member,free_member
                                        ,compareMemberNumOfEvents);
    event_manager->start_date=dateCopy(date);
    event_manager->current_date=dateCopy(date);
    return event_manager;
}

EventManagerResult validDataCheck(EventManager em, char* event_name, Date date, int event_id){
    //null argument check
    if(em==NULL||event_name==NULL||date==NULL){
        return EM_NULL_ARGUMENT;
    }
    //date check
    if(dateCompare(date,em->current_date)<0){
        return EM_INVALID_DATE;
    }
    //id positive check
    if(event_id<0){
        return EM_INVALID_EVENT_ID;
    }
    //name&check check
    PQ_FOREACH(Event,current_event,em->event_list){
        if((dateCompare(current_event->date,date)==0)&&(strcmp(current_event->event_name,event_name)==0)){
            return EM_EVENT_ALREADY_EXISTS;
        }
    }
    //id not taken check
    PQ_FOREACH(Event,current_event,em->event_list){
        if(current_event->event_id==event_id){
            return EM_EVENT_ID_ALREADY_EXISTS;
        }
    }
    return EM_SUCCESS;
}

EventManagerResult emAddEventByDate(EventManager em, char* event_name, Date date, int event_id){
    EventManagerResult result = validDataCheck(em,event_name,date,event_id);
    if(result!=EM_SUCCESS){
        return result;
    }
    Event new_event = malloc(sizeof(struct Event_t));
    if (new_event==NULL){
        return EM_OUT_OF_MEMORY;
    }
    new_event->event_id=event_id;
    new_event->event_name=event_name;
    new_event->first_member=NULL;
    new_event->member_iterator=NULL;
    new_event->date=date;
    pqInsert(em->event_list,new_event,date);
    free(new_event);
    return EM_SUCCESS;
}

EventManagerResult emAddEventByDiff(EventManager em, char* event_name, int days, int event_id){
    if(em==NULL){
        return EM_NULL_ARGUMENT;
    }
    if(days<0){
        return EM_INVALID_DATE;
    }
    if(event_id<0){
        return EM_INVALID_EVENT_ID;
    }
    Date new_date =dateCopy(em->current_date);
    if(new_date==NULL){
        return EM_OUT_OF_MEMORY;
    }
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

static Event findEvent(EventManager em,int event_id){
    PQ_FOREACH(Event,event,em->event_list){
        if(event->event_id==event_id){
            return event;
        }
    }
    return NULL;
}

static Member findMember(EventManager em,int member_id){
    PQ_FOREACH(Member,member,em->member_list){
        if(member->id==member_id){
            return member;
        }
    }
    return NULL;
}

static void disConnectMemberToEvent(EventManager em, Member member, int event_id){
     PQ_FOREACH(Event,event,em->event_list){
         if(event->event_id==event_id){
            Member last_iterator=NULL;
            event->member_iterator=event->first_member;
            while(event->member_iterator!=NULL){
                if(member->id==event->member_iterator->id){
                    if(last_iterator==NULL){ //member is the first
                        Member copy_of_member=event->first_member;
                        event->first_member=event->first_member->next;
                        free_member(copy_of_member);
                        return;
                    }
                    Member copy_of_member=event->member_iterator;
                    last_iterator->next=event->member_iterator->next;
                    free_member(copy_of_member);
                    return;
                }
                last_iterator=event->member_iterator;
                event->member_iterator=event->member_iterator->next;
            }
        }
    }
}

static void disConnectEventToMember(EventManager em, Member member){
    Member copy_of_member = copy_member(member);
    (member->num_of_events)--;
    Member copy_of_new_member = copy_member(member);
    pqChangePriority(em->member_list,member,copy_of_member,copy_of_new_member);
    free_member(copy_of_member);
    free_member(copy_of_new_member);
}

EventManagerResult emRemoveEvent(EventManager em, int event_id){
    if(em==NULL){
        return EM_NULL_ARGUMENT;
    }
    if(event_id<0){
        return EM_INVALID_EVENT_ID;
    }
    Event event=findEvent(em,event_id);
    if(event==NULL){
        return EM_EVENT_NOT_EXISTS;
    }
    Event relevent_event = findEvent(em,event_id);
        relevent_event->member_iterator=relevent_event->first_member;
        while(relevent_event->member_iterator!=NULL){
            Member relevent_member=findMember(em,relevent_event->member_iterator->id);
            disConnectEventToMember(em,relevent_member);
            relevent_event->member_iterator=relevent_event->member_iterator->next;
        }
    pqRemoveElement(em->event_list,event);
    return EM_SUCCESS;
}

EventManagerResult emChangeEventDate(EventManager em, int event_id, Date new_date){
    if(em==NULL||new_date==NULL){
        return EM_NULL_ARGUMENT;
    }
    if(dateCompare(new_date,em->current_date)<0){
        return EM_INVALID_DATE;
    }
    if(event_id<0){
        return EM_INVALID_EVENT_ID;
    }
    Event event=findEvent(em,event_id);
    //checks if theser a conflicted date&&name event in the new date
    PQ_FOREACH(Event,current_event,em->event_list){
        if((dateCompare(current_event->date,new_date)==0)&&(strcmp(current_event->event_name,event->event_name)==0)){
            return EM_EVENT_ALREADY_EXISTS;
        }
    }
    PQ_FOREACH(Event,current_event,em->event_list){
        if (current_event->event_id==event_id){
            if(dateCompare(current_event->date,new_date)==0){
                return EM_EVENT_ALREADY_EXISTS;
            }
            pqChangePriority(em->event_list,current_event,current_event->date,new_date);
            Event event=findEvent(em,event_id);
            dateDestroy(event->date);
            event->date=dateCopy(new_date);
            return EM_SUCCESS;
        }
    }
    return EM_EVENT_ID_NOT_EXISTS;
}

EventManagerResult emAddMember(EventManager em, char* member_name, int member_id){
    if(em==NULL||member_name==NULL){
        return EM_NULL_ARGUMENT;
    }
    if(member_id<0){
        return EM_INVALID_MEMBER_ID;
    }
    if(findMember(em,member_id)!=NULL){
        return EM_MEMBER_ID_ALREADY_EXISTS;
    }
    Member new_member=malloc(sizeof(struct Member_t));
    if(new_member==NULL){
        return EM_OUT_OF_MEMORY;
    }
    new_member->name=member_name;
    new_member->id=member_id;
    new_member->num_of_events=0;
    new_member->next=NULL;
    PriorityQueueResult insert_result=pqInsert(em->member_list,new_member,new_member);
    if(insert_result==PQ_OUT_OF_MEMORY){
        free(new_member);
        return EM_OUT_OF_MEMORY;
    }
    free(new_member);
    return EM_SUCCESS;
}

static bool memberAndEventAlreadyLinked(EventManager em,int member_id, int event_id){
    PQ_FOREACH(Event,event,em->event_list){
        if(event->event_id==event_id){
            event->member_iterator=event->first_member;
            while(event->member_iterator){
                if(event->member_iterator->id==member_id){
                    return true;
                }
                event->member_iterator=event->member_iterator->next;
            }  
        }
    }
    return false;
}

static void connectMemberToEvent(EventManager em, Member member, int event_id){
    Member copy_of_member=copy_member(member);
    copy_of_member->next=NULL;
    PQ_FOREACH(Event,event,em->event_list){
        if(event->event_id==event_id){
            Member last_iterator=NULL;
            if(event->first_member==NULL){
                event->first_member=copy_of_member;
                return;
            }
            event->member_iterator=event->first_member;
            while(event->member_iterator!=NULL){
                if(copy_of_member->id<event->member_iterator->id){
                    if(last_iterator==NULL){ //member should be first
                        copy_of_member->next=event->member_iterator;
                        event->first_member=copy_of_member;
                        return;
                    }
                    copy_of_member->next=event->member_iterator;
                    last_iterator->next=copy_of_member;
                    return;
                }
                last_iterator=event->member_iterator;
                event->member_iterator=event->member_iterator->next;
            }
            last_iterator->next=copy_of_member;
        }
    }
}

static void connectEventToMember(EventManager em, Member member){
    Member copy_of_member = copy_member(member);
    (member->num_of_events)++;
    Member copy_of_new_member = copy_member(member);
    pqChangePriority(em->member_list,member,copy_of_member,copy_of_new_member);
    free_member(copy_of_member);
    free_member(copy_of_new_member);
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
    if(memberAndEventAlreadyLinked(em,member->id,event_id)){
        return EM_EVENT_AND_MEMBER_ALREADY_LINKED;
    }
    connectMemberToEvent(em,member,event_id);
    connectEventToMember(em,member);

    return EM_SUCCESS;
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

    if(findEvent(em,event_id)==NULL){
        return EM_EVENT_ID_NOT_EXISTS;
    }
    Member member=findMember(em,member_id);
    if(member==NULL){
        return EM_MEMBER_ID_NOT_EXISTS;
    }
    if(!memberAndEventAlreadyLinked(em,member->id,event_id)){
        return EM_EVENT_AND_MEMBER_NOT_LINKED;
    }
    disConnectMemberToEvent(em,member,event_id);
    disConnectEventToMember(em,member);
    return EM_SUCCESS;

}

EventManagerResult emTick(EventManager em, int days){
    if(em==NULL){
        return EM_NULL_ARGUMENT;
    }
    if(days<=0){
        return EM_INVALID_DATE;
    }
    for (int i=0; i<days;i++){
        dateTick(em->current_date);
    }
    while((pqGetSize(em->event_list)>0)&&
    (dateCompare(((Event)pqGetFirst(em->event_list))->date,em->current_date)<0)){
        Event relevent_event = (Event)pqGetFirst(em->event_list);
        relevent_event->member_iterator=relevent_event->first_member;
        while(relevent_event->member_iterator!=NULL){
            Member relevent_member=findMember(em,relevent_event->member_iterator->id);
            disConnectEventToMember(em,relevent_member);
            relevent_event->member_iterator=relevent_event->member_iterator->next;
        }
        pqRemove(em->event_list);
    }
    return EM_SUCCESS;
}

int emGetEventsAmount(EventManager em){
    if(em==NULL){
        return EM_IS_NULL;
    }
    return pqGetSize(em->event_list);
}

char* emGetNextEvent(EventManager em){
    if(em==NULL){
        return NULL;
    }
    if((Event)pqGetFirst(em->event_list)==NULL){
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
        fprintf(write_to_file,"%s,%d.%d.%d",current_event->event_name,day,month,year);
        current_event->member_iterator=current_event->first_member;
        while (current_event->member_iterator!=NULL){
            fprintf(write_to_file,",%s",current_event->member_iterator->name);
            current_event->member_iterator=current_event->member_iterator->next;
        }
        fprintf(write_to_file,"\n");     
    }
    fclose(write_to_file);
}

void emPrintAllResponsibleMembers(EventManager em, const char* file_name){
    FILE* write_to_file=fopen(file_name,"w");
    if(write_to_file==NULL){
        return;
    }
    PQ_FOREACH(Member,current_member,em->member_list){
        if(current_member->num_of_events==0){
            fclose(write_to_file);
            return;
        }
        fprintf(write_to_file,"%s,%d\n",current_member->name,current_member->num_of_events);
    }
    fclose(write_to_file);
}
