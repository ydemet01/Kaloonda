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
#include "httprequest.h"


typedef struct thread_node {
    struct thread_node *next;
    pthread_t *thread;
    pthread_cond_t cond;
    char *request;
} THREAD_NODE;

typedef struct {
    THREAD_NODE *head;
    THREAD_NODE *tail;
    pthread_mutex_t poolLock;
    int length;
}THREADPOOL;

THREADPOOL *threadpool;
int threadpoolInit(int threads);
#endif /* ThreadPool_h */
