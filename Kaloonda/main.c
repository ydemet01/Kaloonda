//
//  main.c
//  Kaloonda
//
//  Created by Yiannis Demetriades on 07/04/17.
//  Copyright © 2017 Yiannis Demetriades - Panayiotis Nicolaou. All rights reserved.
//

#include "main.h"


int checkArguments(int argc, char **argv, int *port, unsigned long *num_of_threads){
    if((argc>3) || (argc<2)){
        printf("Wrong arguments passed: ./Kaloonda PORT_NUMBER [NUM_OF_THREADS]\n");
        exit(EXIT_ON_ERROR_SYNTAX);
    }
    *port=atoi(argv[1]);
    if(argc==3){
        unsigned long s=atoi(argv[2]);
        if((s>0) && (s<=sysconf(_SC_THREAD_THREADS_MAX)))
            *num_of_threads=s;
    }

    return EXIT_SUCCESS;
}



int main(int argc, const char * argv[]) {
    int port=8080;
    unsigned long num_of_threads=sysconf(_SC_THREAD_THREADS_MAX);
    checkArguments(argc, argv, &port, &num_of_threads);
    socketEstablishment(port);
    threadpoolInit(num_of_threads);
    webserverInit(port);
    return 0;
}
