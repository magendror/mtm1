#include "priority_queue.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

struct PriorityQueue_t{
    struct PriorityQueue_t next;
    PQElementPriority priority;
    PQElement element;
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
}