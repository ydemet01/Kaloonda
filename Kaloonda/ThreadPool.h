//
//  ThreadPool.h
//  Kaloonda
//
//  Created by Yiannis Demetriades on 07/04/17.
//  Copyright © 2017 Yiannis Demetriades - Panayiotis Nicolaou. All rights reserved.
//

#ifndef ThreadPool_h
#define ThreadPool_h

#include <stdio.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <netinet/in.h>
#include "httprequest.h"
#define perror2(s, e) fprintf(stderr, "%s: %s\n", s, strerror(e))

typedef struct thread_node {
    struct thread_node *next;
    pthread_t *thread;
    pthread_mutex_t nodemutex;
    pthread_cond_t cond;
    
    int newsock;
}THREAD_NODE;

typedef struct {
    THREAD_NODE *head;
    THREAD_NODE *tail;
    pthread_mutex_t poolLock;
    int length;
}THREADPOOL;

int threadpoolInit(unsigned long threads);
int dequeue(THREADPOOL *q, THREAD_NODE **retval);
int enqueue (THREADPOOL *q, THREAD_NODE *node);
void returnThreadpool(THREADPOOL **ret);
#endif /* ThreadPool_h */
