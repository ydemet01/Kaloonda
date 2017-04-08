//
//  main.h
//  find
//
//  Created by Yiannis Demetriades on 27/03/17.
//  Copyright © 2017 Yiannis Demetriades. All rights reserved.
//

#ifndef main_h
#define main_h
#include <stdio.h> //NULL
#include <unistd.h>
#include <stdlib.h>
#include <fnmatch.h>
#include <getopt.h> //getopt
#include <sys/types.h>
#include <dirent.h> // opendir, readdir, closedir
#include <sys/stat.h> // lstat
#include <pwd.h>
#include <grp.h>
#include <limits.h>
#include <stdbool.h> //BOOLEAN
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include "connection.h"
#include "httprequest.h"

#endif /* main_h */
