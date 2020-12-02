#include "event_manager.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>

typedef struct Member_t{
    int id;
    char* name;
    struct Member_t* next;
}*Member;

typedef struct Event_t{
    Date date;
    Member member;
    int id;
    char* data;
    struct Event_t* next; 

}*Event
struct EventManager_t{
    Event event;
    Member member;
}

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
