#include "priority_queue.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#define INPUT_IS_NULL -1
#define ELEMENT_FOR(iterator) \
   pqGetFirst(queue);\
   for(queue->iterator;queue->iterator!=NULL;queue->iterator=queue->iterator->next)

typedef struct element_struct{
    PQElementPriority element_priority;
    PQElement element_data;
    struct element_struct* next;
}*Element;

struct PriorityQueue_t{
    Element element_list;
    CopyPQElement copy_element;
    FreePQElement free_element;
    EqualPQElements equal_elements;
    CopyPQElementPriority copy_priority;
    FreePQElementPriority free_priority;
    ComparePQElementPriorities compare_priorities;
    Element iterator;
    Element first_element;
};

PriorityQueue pqCreate(CopyPQElement copy_element,
                       FreePQElement free_element,
                       EqualPQElements equal_elements,
                       CopyPQElementPriority copy_priority,
                       FreePQElementPriority free_priority,
                       ComparePQElementPriorities compare_priorities){
    if(!pqFuncValid(copy_element,free_element,equal_elements,copy_priority,free_priority,compare_priorities)){
        return NULL;
    }
    PriorityQueue queue=malloc(sizeof(PriorityQueue));
    if (queue==NULL){
        return NULL;
    }
    Element element_list=malloc(sizeof(PriorityQueue));
    if (queue==NULL){
        free(queue);
        return NULL;
    }
    queue->element_list=element_list;
    queue->compare_priorities=compare_priorities;
    queue->copy_priority=copy_priority;
    queue->free_priority=free_priority;
    queue->equal_elements=equal_elements;
    queue->free_element=free_element;
    queue->copy_element=copy_element;
    queue->iterator=NULL;
    queue->first_element=NULL;
    return queue;
}

static bool pqFuncValid(CopyPQElement copy_element,
                       FreePQElement free_element,
                       EqualPQElements equal_elements,
                       CopyPQElementPriority copy_priority,
                       FreePQElementPriority free_priority,
                       ComparePQElementPriorities compare_priorities){

    if (copy_element&&free_element&&equal_elements&&copy_priority&&free_priority&&compare_priorities){
    return true;
    }
    return false;
}

void pqDestroy(PriorityQueue queue){  
    ELEMENT_FOR(iterator){
        queue->free_element(queue->iterator->element_data);
        queue->free_priority(queue->iterator->element_priority);
    }
    free(queue->element_list);
    free(queue);
}

int pqGetSize(PriorityQueue queue){
    if(!queue){
        return INPUT_IS_NULL;
    }
    int pq_size=0;
    ELEMENT_FOR(iterator){
        pq_size=pq_size+1;
    }
    return pq_size;
}

bool pqContains(PriorityQueue queue, PQElement element){
    if(!queue||!element){
            assert(element==NULL||queue==NULL);
            return false;
        }
    ELEMENT_FOR(current_element){
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
        queue->first_element=element;
        assert(queue->element->next==NULL);
        return PQ_SUCCESS;
    }
    PQElement new_element=elementCreate(element,priority);
    ELEMENT_FOR(current_element){
        int compare_result=queue->compare_priorities(new_element->element_data,current_element->element_data);
        int next_compare_result=queue->compare_priorities(new_element->element_data,current_element->next->element_data);
        if((compare_result<=0)&&(next_compare_result>0)){
            new_element->next=current_element->next;
            current_element->next=new_element;
            return PQ_SUCCESS;
        }
        if(compare_result>0){
            assert(compare_priorities(new_element->element_data,queue->first_element->element_data)>0);
            new_element->next=queue->first_element;
            queue->first_element=new_element;
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
    ELEMENT_FOR(current_element){
        if((queue->equal_elements(element,current_element)&&(current_element->element_priority==old_priority))){
            return current_element;
        }   
    }
    return NULL;
}

PQElement pqGetFirst(PriorityQueue queue){
    if(!queue){
        return NULL;
    }
    queue->iterator=queue->first_element;
    return queue->first_element;
}

PQElement pqGetNext(PriorityQueue queue){
    if(!queue){
        return NULL;
    }
    queue->iterator=queue->iterator->next;
    return queue->iterator;
}

PriorityQueue pqCopy(PriorityQueue queue){
    if(queue==NULL){
        return NULL;
    }
    PriorityQueue copy_queue =pqCreate(queue->copy_element,queue->free_element,
                                        queue->equal_elements, queue->copy_priority,
                                        queue->free_priority,queue->compare_priorities);
    ELEMENT_FOR(iterator){
        pqInsert(copy_queue,queue->iterator->element_data,queue->iterator->element_priority);
    }                                    
    return copy_queue;
}

PriorityQueueResult pqRemove(PriorityQueue queue){
    if(queue==NULL){
        return PQ_NULL_ARGUMENT;
    }
    queue->iterator=queue->first_element->next;
    queue->free_element(queue->first_element->element_data);
    queue->free_priority(queue->first_element->element_priority);
    free(queue->first_element);
    queue->first_element=queue->iterator;
}