//
//  ThreadPool.c
//  Kaloonda
//
//  Created by Yiannis Demetriades on 07/04/17.
//  Copyright © 2017 Yiannis Demetriades - Panayiotis Nicolaou. All rights reserved.
//

#include "ThreadPool.h"

THREADPOOL *threadpool;

int enqueue (THREADPOOL *q, THREAD_NODE *node) {

    node->next = NULL;
    if (q->length == 0)
        q->head = q->tail = node;
    else { // append on end
        q->tail->next = node;
        q->tail = node;
    }
    (q->length)++;
    return EXIT_SUCCESS;
}

int dequeue(THREADPOOL *q, THREAD_NODE **retval){
    THREAD_NODE *p=NULL;
    if ((q == NULL) || (q->head == NULL)){
        return EXIT_FAILURE;
    }
    if (retval== NULL) {
        printf("Retval is null");
        return EXIT_FAILURE;
    }

    p = q->head;
    *retval=q->head;
    q->head = q->head->next;
    --(q->length);

    if (q->length == 0) {
        q->tail = NULL;
    }

    return EXIT_SUCCESS;
}

int initTHREADPOOL(THREADPOOL **threadpool) {
    *threadpool = (THREADPOOL *)malloc(sizeof(THREADPOOL));
    if ((*threadpool) == NULL)
        return EXIT_FAILURE;
    (*threadpool)->head = (*threadpool)->tail = NULL;
    (*threadpool)->length = 0;
    pthread_mutex_init(&((*threadpool)->poolLock),NULL);
    return EXIT_SUCCESS;
}

int threadpoolInit(unsigned long threads){

    threadpool=NULL;
    initTHREADPOOL(&threadpool);
    int err;
    unsigned long i=0;
    for (i=0;i<threads;i++) {
        THREAD_NODE *node= (THREAD_NODE*)malloc(sizeof(THREAD_NODE));
        if (node == NULL) {
            return EXIT_FAILURE; // EXIT_CODE_OUT_OF_MEMORY
        }
        pthread_mutex_init(&(node->nodemutex),NULL);
        pthread_cond_init(&(node->cond), NULL);
        pthread_t p;
        if ((err = pthread_create(&p, NULL, threadFunction, (void *) node))) {
            perror2("pthread_create", err);
            return EXIT_FAILURE;
        }
        node->thread=&p;
        enqueue(threadpool, node);
    }
    
    
    return EXIT_SUCCESS;
}

void returnThreadpool(THREADPOOL **ret){
    *ret=threadpool;
}

#ifdef THREADPOOL_TEST
int main(){
    int err=threadpoolInit(10);
    if(err=EXIT_SUCCESS)
        printf("TEST 1 Success: Threadpool Initialized");
    else{
        printf("TEST 1 Failure: Threadpool Initialized");
    }
    THREAD_NODE *node=NULL;
    dequeue(threadpool, &node);
    if(node!=NULL)
        printf("TEST 2 Success: Threadpool Dequeue");
    else{
        printf("TEST 2 Failure: Threadpool Dequeue");
    }
    err=enqueue(threadpool, node)
    if(err=EXIT_SUCCESS)
        printf("TEST 3 Success: Threadpool Enqueue");
    else{
        printf("TEST 3 Failure: Threadpool Enqueue");
    }
}
#endif


