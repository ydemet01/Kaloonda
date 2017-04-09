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
    
    server.sin_family = PF_INET; // Internet domain
    server.sin_addr.s_addr = htonl(INADDR_ANY); // My Internet address
    server.sin_port = htons(port); // The given port
    serverptr = (struct sockaddr *) &server;
    serverlen = sizeof (server);
    
    // Bind socket to address
    if (bind(sock, serverptr, serverlen) < 0) {
        //perror("bind");
        exit(BINDING_FAILED);
    }
    
    // Listen for connections
    if (listen(sock, 5) < 0) { // 5 max. requests in queue
        //perror("listen");
        exit(LISTEN_FAILED);
    }
    
    printf("Listening for connections to port %d\n", port);
    
    return EXIT_SUCCESS;
}


int webserverInit(int wport){
    int err;
    port=wport;
    socketEstablishment(port);

    char buf[1024];
    
    while(1) {
        clientptr = (struct sockaddr *) &client;
        clientlen = sizeof(client);
        /* Accept connection */
        if ((newsock = accept(sock, clientptr, &clientlen)) < 0) {
            //perror("Accept Failed");
            exit(ACCEPT_FAILED);
        }
        /* Using IP address find DNS name (i.e., reverse DNS)*/
        if ((rem = gethostbyaddr((char *) &client.sin_addr.s_addr,sizeof (client.sin_addr.s_addr), client.sin_family)) == NULL) {
            herror("Get host by address failed");
            exit(GET_HOST_FAILED);
        }
        printf("Accepted connection from %s\n", rem -> h_name);

        THREAD_NODE *node=NULL;
        
        if ((err = pthread_mutex_lock(&(threadpool->poolLock)))) { /* lock mutex */
            printf("pthread_mutex_lock: %s\n",strerror(err));
            exit(1);
        }
        if(threadpool->length>0){
            
            dequeue(threadpool, &node);
        }
        if ((err=pthread_mutex_unlock(&(threadpool->poolLock)))) { /* unlock mutex */
            printf("pthread_mutex_unlock: %s\n",strerror(err));
            exit(1);
        }
        
        node->client=client;
        node->clientlen=clientlen;
        node->clientptr=clientptr;
        node->newsock=newsock;
        node->rem=rem;
        
        if ((err=pthread_mutex_unlock(&(node->nodemutex)))) { /* unlock mutex */
            printf("pthread_mutex_unlock: %s\n",strerror(err));
            exit(1);
        }
        
//        if ((err = pthread_cond_signal(&(node->cond)))) {
//            printf("pthread_cond_signal: %s\n",strerror(err));
//            exit(1);
//        }
     
        /*
         /////////
         /////////
         //Assign request to a thread
         /////////
         /////////
         */
    }
    
    return WEBSERVER_INIT_SUCCESS;
}

