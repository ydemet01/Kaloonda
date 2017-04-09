//
//  main.c
//  Kaloonda
//
//  Created by Yiannis Demetriades on 07/04/17.
//  Copyright © 2017 Yiannis Demetriades - Panayiotis Nicolaou. All rights reserved.
//

#include "main.h"


int checkArguments(int argc, char **argv, int *port, int *num_of_threads){
    if((argc>3) || (argc<2)){
        perror("Wrong arguments passed: ./Kaloonda PORT_NUMBER [NUM_OF_THREADS]");
        exit(EXIT_ON_ERROR_SYNTAX);
    }
    *port=atoi(argv[1]);
    if(argc==3){
        int s=atoi(argv[2]);
        if((s>0) && (s<=_SC_THREAD_THREADS_MAX))
            *num_of_threads=s;
    }

    return EXIT_SUCCESS;
}



int main(int argc, const char * argv[]) {
    int port, num_of_threads=_SC_THREAD_THREADS_MAX;
    //checkArguments(argc, argv, &port, &num_of_threads);
    webserverInit(8080);
//    serveRequest();
    
    
    return 0;
}
