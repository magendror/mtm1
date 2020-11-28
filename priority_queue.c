#include "priority_queue.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#define INPUT_IS_NULL -1
struct PriorityQueue_t{
    struct PriorityQueue_t* next;
    PQElementPriority priority;
    PQElement element;
    CopyPQElement copy_element;
    FreePQElement free_element;
    EqualPQElements equal_elements;
    CopyPQElementPriority copy_priority;
    FreePQElementPriority free_priority;
    ComparePQElementPriorities compare_priorities;
};

PriorityQueue pqCreate(CopyPQElement copy_element,
                       FreePQElement free_element,
                       EqualPQElements equal_elements,
                       CopyPQElementPriority copy_priority,
                       FreePQElementPriority free_priority,
                       ComparePQElementPriorities compare_priorities){
    PriorityQueue queue=malloc(sizeof(PriorityQueue));
    queue->element=NULL;
    queue->priority=NULL;
    queue->compare_priorities=compare_priorities;
    queue->copy_priority=copy_priority;
    queue->free_priority=free_priority;
    queue->equal_elements=equal_elements;
    queue->free_element=free_element;
    queue->copy_element=copy_element;
    return queue;
}

void pqDestroy(PriorityQueue queue){  
    while(queue){     
        PriorityQueue to_delete = queue;
        queue=pqGetNext(queue);
        to_delete->free_element(to_delete->element);
        to_delete->free_priority(to_delete->priority);
        free(to_delete);
        }
}
typedef struct element_struct{
    PQElementPriority element_priority;
    PQElement element_data;
    struct element_struct* next;
}*Element;

int pqGetSize(PriorityQueue queue){
    if(!queue){
        return INPUT_IS_NULL;
    }
    int pq_size=0;
    PQ_FOREACH(PQElment,current_element,queue){
        pq_size=pq_size+1;
    }
    return pq_size;
}

bool pqContains(PriorityQueue queue, PQElement element){
    if(!queue||!element){
            assert(element==NULL||queue==NULL);
            return false;
        }
    PQ_FOREACH(PQElment,current_element,queue){
        if(queue->equal_elements(element,current_element)){
            return true;
        }
    }
    return false;
}

PriorityQueueResult pqInsert(PriorityQueue queue, PQElement element, PQElementPriority priority){
    if(!queue||!element||!priority){
        return PQ_NULL_ARGUMENT;
    }
    if(pqGetSize(queue)==0){
        assert((queue->element=NULL)&&(queue->priority=NULL));
        queue->element=elementCreate(element,priority);
        if(queue->element==NULL)(
            return PQ_OUT_OF_MEMORY;
        )
        assert(queue->element->next==NULL);
        return PQ_SUCCESS;
    }
    PQElement new_element=elementCreate(element,priority);
    PQ_FOREACH(PQElment,current_element,queue){
        int compare_result=queue->compare_priorities(new_element->element_data,current_element->element_data);
        int next_compare_result=queue->compare_priorities(new_element->element_data,pqGetNext(queue)->element_data);
        if((compare_result<=0)&&(next_compare_result>0)){
            new_element->next=pqGetNext(queue);
            current_element->next=new_element;
            return PQ_SUCCESS;
        }
    }
}

static PQElement elementCreate(PQElement element, PQElementPriority priority){
    Element new_element=malloc(sizeof(*new_element));
    if(!new_element){
        return NULL;
    }
    new_element->element_priority=priority;
    new_element->element_data=element;
    new_element->next=NULL;
    return new_element;
}

PriorityQueueResult pqChangePriority(PriorityQueue queue, PQElement element, PQElementPriority old_priority, PQElementPriority new_priority){
    if((!queue)||(!element)||(!old_priority)||(!new_priority)){
        return PQ_NULL_ARGUMENT;
    }
    PQElement change_element=find_element(queue,element,old_priority);
    if(!new_element){
        return PQ_ELEMENT_DOES_NOT_EXISTS;
    }
    PQElement first_element=pqGetFirst(queue);
    change_element->next=first_element;
    queue->element=change_element;
    PriorityQueueResult remove_result=pqRemove(queue);
    assert(remove_result==PQ_SUCCESS);
    PriorityQueueResult new_insert_result=pqInsert(queue,element,new_priority);
    assert(new_insert_result==PQ_SUCCESS);
    return PQ_SUCCESS;
}
static PQElement find_element(PriorityQueue queue, PQElement element, PQElementPriority old_priority){
    PQ_FOREACH(PQElment,current_element,queue){
        if((queue->equal_elements(element,current_element)&&(current_element->element_priority==old_priority)){
            return current_element;
        }
        return NULL;
}

