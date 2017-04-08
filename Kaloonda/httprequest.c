//
//  httprequest.c
//  Kaloonda
//
//  Created by Yiannis Demetriades on 08/04/17.
//  Copyright © 2017 Yiannis Demetriades - Panayiotis Nicolaou. All rights reserved.
//

#include "httprequest.h"



void *serveRequest(){
    char *s="GET /index.html HTTP/1.1\r\n";
    char s1[30], s2[30], s3[30], s4[30];
    sscanf(s, "%s %s %s\r\n", s1, s2, s3);
//    strtok(
    printf("%s\n%s\n%s\n%s\n", s1, s2, s3, s4);
    
    return EXIT_SUCCESS;
}
