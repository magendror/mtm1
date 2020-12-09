#include "event_manager.h"
#include "date.h"
#include "priority_queue.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#define EM_IS_NULL -1

typedef struct Member_t{
    int id;
    char* name;
    PQElementPriority priority;
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
    //Member member_iterator;
    //Event iterator;
    Date start_date;
    Date current_date;
};

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
    new_event->first_member=NULL;
    ((Event)event)->member_iterator=((Event)event)->first_member;
    while(((Event)event)->member_iterator!=NULL){
        Member new_member=malloc(sizeof(struct Member_t));
        if(new_member==NULL){
            return NULL;
        }
        new_member=copy_member(((Event)event)->member_iterator);
        if(new_event->first_member=NULL){
            new_event->first_member=new_member;
            new_member_iterator=new_event->first_member;
        }
        else{
            new_member_iterator=new_member; 
        }
        ((Event)event)->member_iterator=((Event)event)->member_iterator->next;
        new_member_iterator=new_member_iterator->next;
    }
    new_event->date=new_date;
    return new_event;
}

static void freeEvent(PQElement event){
    free(((Event)event)->event_name);
    dateDestroy(((Event)event)->date);
    ((Event)event)->member_iterator=((Event)event)->first_member;
    ///////////////////////////////////////////////////////////////////////////////////////////////
    printf("%d",((Event)event)->first_member->id);
    ///////////////////////////////////////////////////////////////////////////////////////////////
    Member local_iterator;
    while(((Event)event)->member_iterator!=NULL){
        local_iterator=((Event)event)->member_iterator->next;
        free(((Event)event)->member_iterator->name);
        free(((Event)event)->member_iterator);
        ((Event)event)->member_iterator=local_iterator;
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
    free(em);
}

static PQElement copy_member(PQElement member){
    Member new_member=malloc(sizeof(struct Member_t));
    if(new_member==NULL){
        return NULL;
    }
    new_member->name=malloc(sizeof(char)*(strlen(((Member)member)->name)+1));
    if(new_member->name==NULL){
        free(new_member);
        return NULL;
    }
    new_member=member;
    assert(((Member)member)->name==new_member->name);
    return new_member;
}

static void free_member(PQElement member){
    free(((Member)member)->name);
    free(member);
}

static bool equal_member(PQElement member1,PQElement member2){
    bool equal_id=(((Member)member1)->id==((Member)member2)->id);
    bool equal_name=(strcmp(((Member)member1)->name,((Member)member2)->name)==0);
    return ((equal_id)&&(equal_name));
}

static PQElementPriority copyMemberNumOfEvents(PQElementPriority member_num_of_events){
    int* new_num_of_events = malloc(sizeof(int)); 
        if(new_num_of_events==NULL){
            return NULL;
        }
        *new_num_of_events=((Member)member_num_of_events)->num_of_events;
    return new_num_of_events;
}

static void freeMemberNumOfEvents(PQElementPriority member_num_of_events){
    free(member_num_of_events);
}

static int compareMemberNumOfEvents(PQElement member1,PQElement member2){
    if(((Member)member1)->num_of_events==((Member)member2)->num_of_events){
       return ((Member)member1)->id-((Member)member2)->id;
   }
   return((((Member)member1)->num_of_events)-(((Member)member2)->num_of_events));
}

EventManager createEventManager(Date date){
    EventManager event_manager=malloc(sizeof(struct EventManager_t));
    if (event_manager==NULL){
        return NULL;
    }
    event_manager->event_list=pqCreate(copyEvent,freeEvent,equalEventGeneric,
                                        dateCopyGeneric,dateFree,dateCompareReversed);
    event_manager->member_list=pqCreate(copy_member,free_member,equal_member
                                        ,copyMemberNumOfEvents,freeMemberNumOfEvents
                                        ,compareMemberNumOfEvents);
    //event_manager->member_iterator=NULL;
    event_manager->start_date=date;
    event_manager->current_date=date;
    //event_manager->iterator=NULL;
    return event_manager;
}

EventManagerResult validDataCheck(EventManager em, char* event_name, Date date, int event_id){
    //null argument check // ask what is about
    if(em==NULL||event_name==NULL||date==NULL){
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
            return EM_EVENT_ALREADY_EXISTS;
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
    Event new_event = malloc(sizeof(struct Event_t));
    if (new_event==NULL){
        return EM_OUT_OF_MEMORY;
    }
    new_event->date=date;
    new_event->event_id=event_id;
    new_event->event_name=event_name;
    new_event->first_member=NULL;
    new_event->member_iterator=NULL;
    pqInsert(em->event_list,new_event,date);
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
    pqRemoveElement(em->event_list,event);
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

EventManagerResult emAddMember(EventManager em, char* member_name, int member_id){
    if(em==NULL||member_name==NULL){
        return EM_NULL_ARGUMENT;
    }
    if(member_id<0){
        return EM_INVALID_MEMBER_ID;
    }
    Member new_member=malloc(sizeof(struct Member_t));
    if(new_member==NULL){
        return EM_OUT_OF_MEMORY;
    }
    new_member->name=malloc(sizeof(char)*(strlen(member_name)+1));
    if(new_member->name==NULL){
        free(new_member);
        return EM_OUT_OF_MEMORY;
    }
    new_member->name=member_name;
    new_member->id=member_id;
    new_member->num_of_events=0;
    new_member->priority=&(new_member->num_of_events);
    PriorityQueueResult insert_result=pqInsert(em->member_list,new_member,new_member->priority);
    if(insert_result==PQ_OUT_OF_MEMORY){
        free(new_member->name);
        free(new_member);
        return EM_OUT_OF_MEMORY;
    }
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
            }  
        }
    }
    return false;
}

static void connectMemberToEvent(EventManager em, Member member, int event_id){
     PQ_FOREACH(Event,event,em->event_list){
         if(event->event_id==event_id){
            Member last_iterator=NULL;
            event->member_iterator=event->first_member;
            while(event->member_iterator!=NULL){
                if(member->id<event->member_iterator->id){
                    if(last_iterator==NULL){ //member should be first
                        member->next=event->member_iterator;
                        event->first_member=member;
                        return;
                    }
                    member->next=event->member_iterator;
                    last_iterator->next=member;
                    return;
                }
                last_iterator=event->member_iterator;
                event->member_iterator=event->member_iterator->next;
            }
            last_iterator->next=member;
         }
     }
}

static void connectEventToMember(EventManager em, Member member){
    PQElementPriority old_priority=&(member->num_of_events);
    member->num_of_events++;
    int new_priority=member->num_of_events;
    PQElementPriority p_new_priority=&new_priority;
    pqChangePriority(em->member_list,member,old_priority,p_new_priority);
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

static void disConnectMemberToEvent(EventManager em, Member member, int event_id){
     PQ_FOREACH(Event,event,em->event_list){
         if(event->event_id==event_id){
            Member last_iterator=NULL;
            while(event->member_iterator!=NULL){
                if(member->id==event->member_iterator->id){
                    if(last_iterator==NULL){ //member is the first
                        event->first_member=member->next;
                        return;
                    }
                    last_iterator->next=member->next;
                    return;
                }
                last_iterator=event->member_iterator;
                event->member_iterator=event->member_iterator->next;
            }
         }
     }
}

static void disConnectEventToMember(EventManager em, Member member){
    PQElementPriority old_priority=&(member->num_of_events);
    member->num_of_events--;
    int new_priority=member->num_of_events;
    PQElementPriority p_new_priority=&new_priority;
    pqChangePriority(em->member_list,member,old_priority,p_new_priority);
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

    if(!findEvent(em,event_id)){
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
            return;
        }
        fprintf(write_to_file,"%s,%d\n",current_member->name,current_member->num_of_events);
    }
    fclose(write_to_file);
}
