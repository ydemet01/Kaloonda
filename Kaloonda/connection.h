//
//  connection.h
//  Kaloonda
//
//  Created by Yiannis Demetriades on 07/04/17.
//  Copyright © 2017 Yiannis Demetriades - Panayiotis Nicolaou. All rights reserved.
//

#ifndef connection_h
#define connection_h

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "signals.h"
#include "ThreadPool.h"

THREADPOOL *threadpool;
int webserverInit(int port);

#endif /* connection_h */
