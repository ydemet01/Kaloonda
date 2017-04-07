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
        perror("socket");
        exit(1);
    }
    
    server.sin_family = PF_INET; // Internet domain
    server.sin_addr.s_addr = htonl(INADDR_ANY); // My Internet address
    server.sin_port = htons(port); // The given port
    serverptr = (struct sockaddr *) &server;
    serverlen = sizeof (server);
    
    // Bind socket to address
    if (bind(sock, serverptr, serverlen) < 0) {
        perror("bind");
        exit(1);
    }
    
    // Listen for connections
    if (listen(sock, 5) < 0) { // 5 max. requests in queue
        perror("listen");
        exit(1);
    }
    
    printf("Listening for connections to port %d\n", port);
    
    return EXIT_SUCCESS;
}


int webserverInit(int wport){
    int err;
    port=wport;
    if((err=socketEstablishment(port))!=EXIT_SUCCESS){
        perror("Socket could not be established");
        exit(0);
    }

    
    
    while(1) {
        clientptr = (struct sockaddr *) &client;
        clientlen = sizeof(client);
        /* Accept connection */
        if ((newsock = accept(sock, clientptr, &clientlen)) < 0) {
            perror("accept"); exit(1);
        }
        /* Using IP address find DNS name (i.e., reverse DNS)*/
        if ((rem = gethostbyaddr((char *) &client.sin_addr.s_addr,sizeof (client.sin_addr.s_addr), client.sin_family)) == NULL) {
            herror("gethostbyaddr"); // herror(): Similar to perror but uses the h_errno variable (set by name resolution functions to return error values).
            exit(1);
        }
        printf("Accepted connection from %s\n", rem -> h_name);
     
        /*
         /////////
         /////////
         //Assign request to a thread
         /////////
         /////////
         */
    }
    
    return EXIT_SUCCESS;
}

