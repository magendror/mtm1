#include "priority_queue.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

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