#include "test_utilities.h"
#include "../priority_queue.h"
#include <stdlib.h>

#define NUMBER_TESTS 4

static PQElementPriority copyIntGeneric(PQElementPriority n) {
    if (!n) {
        return NULL;
    }
    int *copy = malloc(sizeof(*copy));
    if (!copy) {
        return NULL;
    }
    *copy = *(int *) n;
    return copy;
}

static void freeIntGeneric(PQElementPriority n) {
    free(n);
}

static int compareIntsGeneric(PQElementPriority n1, PQElementPriority n2) {
    return (*(int *) n1 - *(int *) n2);
}

static bool equalIntsGeneric(PQElementPriority n1, PQElementPriority n2) {
    return *(int *) n1 == *(int *) n2;
}

bool testPQCreateDestroy() {
    bool result = true;

    PriorityQueue pq = pqCreate(copyIntGeneric, freeIntGeneric, equalIntsGeneric, copyIntGeneric, freeIntGeneric, compareIntsGeneric);
    ASSERT_TEST(pq != NULL, returnPQCreateDestroy);
    ASSERT_TEST(pqGetSize(pq) == 0,destroyPQCreateDestroy);
    ASSERT_TEST(pqGetFirst(pq) == NULL,destroyPQCreateDestroy);

destroyPQCreateDestroy:
    pqDestroy(pq);
returnPQCreateDestroy:
    return result;
}

bool testPQInsertAndSize() {
    bool result = true;
    PriorityQueue pq = pqCreate(copyIntGeneric, freeIntGeneric, equalIntsGeneric, copyIntGeneric, freeIntGeneric, compareIntsGeneric);
    ASSERT_TEST(pqGetSize(pq) == 0,destroyPQInsertAndSize);
    int to_add = 1;
    ASSERT_TEST(pqInsert(pq, &to_add, &to_add) == PQ_SUCCESS,destroyPQInsertAndSize);
    ASSERT_TEST(pqGetSize(pq) == 1,destroyPQInsertAndSize);

destroyPQInsertAndSize:
    pqDestroy(pq);
    return result;
}

bool testPQGetFirst() {
    bool result = true;
    PriorityQueue pq = pqCreate(copyIntGeneric, freeIntGeneric, equalIntsGeneric, copyIntGeneric, freeIntGeneric, compareIntsGeneric);
    ASSERT_TEST(pqGetSize(pq) == 0, destroyPQGetFirst);
    int to_add = 1;
    ASSERT_TEST(pqInsert(pq, &to_add, &to_add) == PQ_SUCCESS, destroyPQGetFirst);
    int* first_value = pqGetFirst(pq);
    ASSERT_TEST(first_value != NULL, destroyPQGetFirst);
    ASSERT_TEST(*first_value == to_add, destroyPQGetFirst);

destroyPQGetFirst:
    pqDestroy(pq);
    return result;
}

bool testPQIterator() {
    bool result = true;
    PriorityQueue pq = pqCreate(copyIntGeneric, freeIntGeneric, equalIntsGeneric, copyIntGeneric, freeIntGeneric, compareIntsGeneric);

    int max_value = 10;

    for(int i=0; i< max_value; i++){
        ASSERT_TEST(pqInsert(pq, &i, &i) == PQ_SUCCESS, destroyPQIterator);
    }
    int i = 0;
    PQ_FOREACH(int*, iter, pq) {
        if (i != max_value) {
            ASSERT_TEST(iter != NULL,destroyPQIterator);
        } else {
            ASSERT_TEST(iter == NULL,destroyPQIterator);
        }
        i++;
    }

destroyPQIterator:
    pqDestroy(pq);
    return result;
}

bool (*tests[]) (void) = {
        testPQCreateDestroy,
        testPQInsertAndSize,
        testPQGetFirst,
        testPQIterator
};

const char* testNames[] = {
        "testPQCreateDestroy",
        "testPQInsertAndSize",
        "testPQGetFirst",
        "testPQIterator"
};

int main(int argc, char *argv[]) {
    PriorityQueue pq = pqCreate(copyIntGeneric, freeIntGeneric, equalIntsGeneric, copyIntGeneric, freeIntGeneric, compareIntsGeneric);
    int to_add1 = 1;
    int to_add2 = 2;
    int to_add3 = 3;
    pqInsert(pq,&to_add1,&to_add1);
    pqInsert(pq,&to_add2,&to_add2);
    pqInsert(pq,&to_add3,&to_add3);
    PQ_FOREACH(int*,iterator,pq){
        if(*iterator==2){
            *iterator=4;
        }
    }
    PQ_FOREACH(int*,iterator,pq){
        printf("%d",*iterator);
    }  
    return 0;
}
