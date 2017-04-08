//
//  ThreadPool.c
//  Kaloonda
//
//  Created by Yiannis Demetriades on 07/04/17.
//  Copyright © 2017 Yiannis Demetriades - Panayiotis Nicolaou. All rights reserved.
//

#include "ThreadPool.h"

int enqueue (THREADPOOL *q,THREAD_NODE node) {

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

int dequeue(THREADPOOL *q, THREAD_NODE *retval){
    THREAD_NODE *p=NULL;
// copy pointer used for free()
    if ((q == NULL) || (q->head == NULL)){
//        printf("Sorry, THREADPOOL is empty\n");
        return EXIT_FAILURE; //return EXIT_CODE_DEADPOOL_NOTFOUND
    }
    if (retval== NULL) {
        printf("Retval is null");
        return EXIT_FAILURE;
    }

    p = q->head;
    *retval= q->head->pthread_t;
    q->head = q->head->next;
    free(p);
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
    return EXIT_SUCCESS;
}

int threadpoolInit(int threads){

    THREADPOOL *threadpool=NULL;
    initTHREADPOOL(&threadpool);
    int i=0;
    for (i=0;i<threads;i++) {
        THREAD_NODE *node= (THREAD_NODE*)malloc(sizeof(THREAD_NODE));
        if (*node == NULL) {
            return EXIT_FAILURE; // EXIT_CODE_OUT_OF_MEMORY
        }
        pthread_t p;
        if (err = pthread_create(&p, NULL, serveRequest(),node)) {
            perror2("pthread_create", err);
            exit(1);
        }
        node->thread=p;
        enqueue(threadpool,node);

    }
    
    
    return EXIT_SUCCESS;
}
