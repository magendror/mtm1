#include "priority_queue.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>

#define INPUT_IS_NULL -1
#define ELEMENT_FOR(queue) \
   pqGetFirst(queue);\
   for(;queue->iterator!=NULL;queue->iterator=queue->iterator->next)

typedef struct element_struct{
    PQElementPriority element_priority;
    PQElement element_data;
    struct element_struct* next;
}*Element;

struct PriorityQueue_t{
    CopyPQElement copy_element;
    FreePQElement free_element;
    EqualPQElements equal_elements;
    CopyPQElementPriority copy_priority;
    FreePQElementPriority free_priority;
    ComparePQElementPriorities compare_priorities;
    Element iterator;
    Element first_element;
};

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
    queue->first_element=NULL;
    queue->compare_priorities=compare_priorities;
    queue->copy_priority=copy_priority;
    queue->free_priority=free_priority;
    queue->equal_elements=equal_elements;
    queue->free_element=free_element;
    queue->copy_element=copy_element;
    queue->iterator=NULL;
    return queue;
}

void pqDestroy(PriorityQueue queue){  
    pqClear(queue);
    free(queue);
}

int pqGetSize(PriorityQueue queue){
    if(!queue){
        return INPUT_IS_NULL;
    }
    if(queue->first_element==NULL){
        return 0;
    }
    int pq_size=0;
    ELEMENT_FOR(queue){
        pq_size=pq_size+1;
    }
    queue->iterator=NULL;
    return pq_size;
}

static Element elementCreate(PriorityQueue queue,PQElement element, PQElementPriority priority){
    Element new_element=malloc(sizeof(Element));
    if(!new_element){
        return NULL;
    }
    new_element->element_priority=queue->copy_priority(priority);
    if(new_element->element_priority==NULL){
        return NULL;
    }
    new_element->element_data=queue->copy_element(element);
        if(new_element->element_data==NULL){
            queue->free_priority(new_element->element_priority);
            return NULL;
    }
    new_element->next=NULL;
    return new_element;
}

PriorityQueueResult pqInsert(PriorityQueue queue, PQElement element, PQElementPriority priority){
    if(!queue||!element||!priority){
        return PQ_NULL_ARGUMENT;
    }
    Element new_element=elementCreate(queue,element,priority);
        if(queue->first_element==NULL){
            return PQ_OUT_OF_MEMORY;
        }
    if(pqGetSize(queue)==0){
        assert(queue->first_element==NULL);
        queue->first_element=new_element;
        assert(queue->first_element->next==NULL);
        queue->iterator=NULL;
        return PQ_SUCCESS;
    }
    int compare_result=queue->compare_priorities(new_element->element_priority,queue->first_element->element_priority);  
    if(compare_result>0){
            new_element->next=queue->first_element;
            queue->first_element=new_element;
            queue->iterator=NULL;
            return PQ_SUCCESS;
    }
    Element last_iterator=NULL;
    ELEMENT_FOR(queue){
        if(queue->compare_priorities(new_element->element_priority,queue->iterator->element_priority)<0){
            new_element->next=queue->iterator;
            last_iterator->next=new_element;
            queue->iterator=NULL;
            return PQ_SUCCESS;
        }
        last_iterator=queue->iterator;
    }
    //should not reach here
    return PQ_NULL_ARGUMENT;
}

static Element findElementFollowing(PriorityQueue queue, PQElement element, PQElementPriority priority){
    if((queue->equal_elements(element,queue->first_element->element_data)&&
            (queue->first_element->element_priority==priority))){
                return queue->first_element;
            }
    ELEMENT_FOR(queue){
        if(queue->iterator->next==NULL){
            return NULL;
        }
        else if((queue->equal_elements(element,queue->iterator->next->element_data)&&
            (queue->iterator->next->element_priority==priority))){
            return queue->iterator;
        }   
    }
    return NULL;
}

PriorityQueueResult pqChangePriority(PriorityQueue queue, PQElement element, PQElementPriority old_priority, PQElementPriority new_priority){
    if((!queue)||(!element)||(!old_priority)||(!new_priority)){
        return PQ_NULL_ARGUMENT;
    }
    Element change_element_following=findElementFollowing(queue,element,old_priority);
    if(!change_element_following){
        queue->iterator=NULL;
        return PQ_ELEMENT_DOES_NOT_EXISTS;
    }
    pqInsert(queue,change_element_following->next->element_data,new_priority);
    Element to_be_deleted = change_element_following->next;
    change_element_following->next=change_element_following->next->next;
    queue->free_element(to_be_deleted->element_data);
    queue->free_priority(to_be_deleted->element_priority);
    free(to_be_deleted);
    queue->iterator=NULL;
    return PQ_SUCCESS;
}



PQElement pqGetFirst(PriorityQueue queue){
    if(!queue){
        return NULL;
    }
    queue->iterator=queue->first_element;
    if(queue->first_element==NULL){
        return NULL;
    }
    return queue->first_element->element_data;
}

PQElement pqGetNext(PriorityQueue queue){
    if(queue==NULL||queue->iterator==NULL||queue->iterator->next==NULL){
        return NULL;
    }
    queue->iterator=queue->iterator->next;
    return queue->iterator->element_data;
}

PriorityQueue pqCopy(PriorityQueue queue){
    if(queue==NULL){
        return NULL;
    }
    PriorityQueue copy_queue =pqCreate(queue->copy_element,queue->free_element,
                                        queue->equal_elements, queue->copy_priority,
                                        queue->free_priority,queue->compare_priorities);
    ELEMENT_FOR(queue){
        pqInsert(copy_queue,queue->iterator->element_data,queue->iterator->element_priority);
    }
    assert(queue->iterator==NULL&&copy_queue->iterator==NULL);
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
    queue->iterator=NULL;
    return PQ_SUCCESS;
}

static Element findElementFollowingNoPriority(PriorityQueue queue, PQElement element){
    if(queue->equal_elements(element,queue->first_element->element_data)){
                return queue->first_element;
            }
    ELEMENT_FOR(queue){
        if(queue->iterator->next==NULL){
            return NULL;
        }
        else if(queue->equal_elements(element,queue->iterator->next->element_data)){
            return queue->iterator;
        }   
    }
    //should not reach here
    return NULL;
}

PriorityQueueResult pqRemoveElement(PriorityQueue queue, PQElement element){
    if(!queue||!element){
        assert(element==NULL||queue==NULL);
        return PQ_NULL_ARGUMENT;
    }
    Element delete_element_following = findElementFollowingNoPriority(queue,element);
    if (delete_element_following==NULL){
        return PQ_ELEMENT_DOES_NOT_EXISTS;
    }
    Element to_be_deleted=delete_element_following->next;
    queue->free_element(to_be_deleted->element_data);
    queue->free_priority(to_be_deleted->element_priority);
    delete_element_following->next=delete_element_following->next->next;
    free(to_be_deleted);
    queue->iterator=NULL;
    return PQ_SUCCESS;
}

static Element pqContainsAndWhere(PriorityQueue queue, PQElement element){
    if(!queue||!element){
        assert(element==NULL||queue==NULL);
        return NULL;
    }
    ELEMENT_FOR(queue){
        if(queue->equal_elements(element,queue->iterator->element_data)){
            return queue->iterator;
        }
    }
    return NULL;
}

bool pqContains(PriorityQueue queue, PQElement element){
    Element present_iterator=queue->iterator;
    if(pqContainsAndWhere(queue,element)==NULL){
        queue->iterator=present_iterator;
        return false;
    }
    queue->iterator=present_iterator;
    return true;
}

PriorityQueueResult pqClear(PriorityQueue queue){
    if(queue==NULL){
        return PQ_NULL_ARGUMENT;
    }
    queue->iterator=queue->first_element;
    while(queue->iterator!=NULL){
        Element to_delete = queue->iterator;
        queue->iterator=queue->iterator->next;
        queue->free_priority(to_delete->element_priority);
        queue->free_element(to_delete->element_data);
        free(queue->iterator);
    }
    return PQ_SUCCESS;
}