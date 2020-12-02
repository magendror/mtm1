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

struct EventManager_t{
    Date date;
    int id;
    char* data;
    Member member;
    EventManager next;
}

EventManager createEventManager(Date date){
    EventManager event_manager=malloc(sizeof(EventManager));
    if (event_manager==NULL){
        return NULL;
    }
    event_manager->date=date;
    event_manager->id=0;
    event_manager->data=NULL;
    event_manager->member=NULL;
    event_manager->next=NULL;
    return event_manager;
}

void destroyEventManager(EventManager em){
    free(em->date);
    free(em->member);
    free(em);
}
