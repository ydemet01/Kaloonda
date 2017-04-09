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
//#include <sys/types.h>
//#include <sys/socket.h>
#include <netinet/in.h>
//#include <netdb.h>
#include "httprequest.h"


typedef struct thread_node {
    struct thread_node *next;
    pthread_t *thread;
    pthread_mutex_t nodemutex;
//    pthread_cond_t cond;
//    char *request;
    
    int newsock, clientlen;
    struct sockaddr_in client;
    struct sockaddr *clientptr;
    struct hostent *rem;
}THREAD_NODE;

typedef struct {
    THREAD_NODE *head;
    THREAD_NODE *tail;
    pthread_mutex_t poolLock;
    int length;
}THREADPOOL;

THREADPOOL *threadpool;
int threadpoolInit(int threads);
int dequeue(THREADPOOL *q, THREAD_NODE **retval);
int enqueue (THREADPOOL *q, THREAD_NODE *node);
#endif /* ThreadPool_h */
