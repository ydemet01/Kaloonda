//
//  httprequest.c
//  Kaloonda
//
//  Created by Yiannis Demetriades on 08/04/17.
//  Copyright © 2017 Yiannis Demetriades - Panayiotis Nicolaou. All rights reserved.
//

#include "httprequest.h"

char *sgets( char * str, int num, char **input )
{
    char *next = *input;
    int  numread = 0;
    
    while ( numread + 1 < num && *next ) {
        int isnewline = ( *next == '\n' );
        *str++ = *next++;
        numread++;
        // newline terminates the line but is included
        if ( isnewline )
            break;
    }
    
    if ( numread == 0 )
        return NULL;  // "eof"
    
    // must have hit the null terminator or end of line
    *str = '\0';  // null terminate this tring
    // set up input for next call
    *input = next;
    return str;
}


void *serveRequest(){
    char *request="GET /index.html HTTP/1.1\r\nUser-Agent: My-web-browser\r\nHost: astarti.cs.ucy.ac.cy:30000\r\nConnection: keep-alive\r\n\r\n";
    char s[100];
    
    char function[30], link[30], protocolName[30], protocolVersion[30];
    char *token;
    bool keepAlive=true, b=true;

    while (sgets(s, sizeof(s), &request) != 0){
        if(b){
            sscanf(s, "%s %s %s\r\n", function, link, protocolName);
            
            token = strtok(protocolName, "/");
            sprintf(protocolName, "%s", token);
            token = strtok(NULL, "/");
            sprintf(protocolVersion, "%s", token);
            b=false;
        }
        else{
            char t1[30], t2[30];
            sscanf(s, "%s %s\r\n", t1, t2);
        //    if(strcmp(t1,"User-Agent:")==0){
        //        continue;
        //    }
        //    else if(strcmp(t1,"Host:")==0){
        //        continue;
        //    }
        //    else if(strcmp(t1,"Connection:")==0){
            if(strcmp(t1,"Connection:")==0){
                if(strcmp(t2, "close")==0)
                    keepAlive=false;
            }
        }
    }
    
    printf("%s\n%s\n%s\n%s\n%d\n", function, link, protocolName, protocolVersion, keepAlive);
    
    return EXIT_SUCCESS;
}
