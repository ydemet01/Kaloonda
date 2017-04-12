//
//  httprequest.h
//  Kaloonda
//
//  Created by Yiannis Demetriades on 08/04/17.
//  Copyright © 2017 Yiannis Demetriades - Panayiotis Nicolaou. All rights reserved.
//

#ifndef httprequest_h
#define httprequest_h

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "signals.h"
#include "ThreadPool.h"
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>

void *threadFunction(void *);

#endif /* httprequest_h */
