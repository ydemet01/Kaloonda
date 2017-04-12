//
//  connection.c
//  Kaloonda
//
//  Created by Yiannis Demetriades on 07/04/17.
//  Copyright © 2017 Yiannis Demetriades - Panayiotis Nicolaou. All rights reserved.
//


#include "connection.h"

int port, sock, newsock, serverlen, clientlen;
struct sockaddr_in server, client;
struct sockaddr *serverptr, *clientptr;
struct hostent *rem;


int socketEstablishment(int port){
    /* Create socket */
    if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
//        //perror("Socket could not be established");
        exit(1);
    }
    const int optVal=1;
    const socklen_t optLen=sizeof(optVal);
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (void *) &optVal, optLen);
    
    server.sin_family = PF_INET; // Internet domain
    server.sin_addr.s_addr = htonl(INADDR_ANY); // My Internet address
    server.sin_port = htons(port); // The given port
    serverptr = (struct sockaddr *) &server;
    serverlen = sizeof (server);
    
    // Bind socket to address
    if (bind(sock, serverptr, serverlen) < 0) {
        perror("bind");
        exit(BINDING_FAILED);
    }
    
    // Listen for connections
    if (listen(sock, 5) < 0) { // 5 max. requests in queue
        perror("listen");
        exit(LISTEN_FAILED);
    }
    
    printf("Listening for connections to port %d\n", port);
    
    return EXIT_SUCCESS;
}


int webserverInit(int wport){
    int err;
    port=wport;
    
    while(1) {
        clientptr = (struct sockaddr *) &client;
        clientlen = sizeof(client);
        /* Accept connection */
        if ((newsock = accept(sock, clientptr, &clientlen)) < 0) {
            perror("Accept Failed");
            exit(ACCEPT_FAILED);
        }
        /* Using IP address find DNS name (i.e., reverse DNS)*/
        if ((rem = gethostbyaddr((char *) &client.sin_addr.s_addr,sizeof (client.sin_addr.s_addr), client.sin_family)) == NULL) {
            herror("Get host by address failed");
            exit(GET_HOST_FAILED);
        }
        printf("Accepted connection from %s\n", rem -> h_name);

        THREAD_NODE *node=NULL;
        THREADPOOL *threadpool=NULL;
        returnThreadpool(&threadpool);
        if ((err = pthread_mutex_lock(&(threadpool->poolLock)))) { /* lock mutex */
            printf("pthread_mutex_lock: %s\n",strerror(err));
            exit(1);
        }
        if(threadpool->length>0){
            dequeue(threadpool, &node);
        }
        else
            continue;
        if ((err=pthread_mutex_unlock(&(threadpool->poolLock)))) { /* unlock mutex */
            printf("pthread_mutex_unlock: %s\n",strerror(err));
            exit(1);
        }
        node->newsock=newsock;
        
        if ((err = pthread_mutex_lock(&(node->nodemutex)))) { /* lock mutex */
            printf("pthread_mutex_lock: %s\n",strerror(err));
            exit(1);
        }
        if((err=pthread_cond_signal(&(node->cond)))){
            printf("pthread_mutex_unlock:%s\n", strerror(err));
            exit(1);
        }
        if ((err=pthread_mutex_unlock(&(node->nodemutex)))) { /* unlock mutex */
            printf("pthread_mutex_unlock: %s\n",strerror(err));
            exit(1);
        }
    }
    
    return WEBSERVER_INIT_SUCCESS;
}

#ifdef CONNECTION_TEST
int main(){
    int err=webserverInit(8080);
    if(err=WEBSERVER_INIT_SUCCESS)
        printf("TEST 1 Success: Webserver Initialized");
    else{
        printf("TEST 1 Failure: Webserver Initialized");
    }
}
#endif

