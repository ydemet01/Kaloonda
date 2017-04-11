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
//    while(*next!='\r' && *next+1!='\n'){
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

void get_filename_ext(char *filename, char **extension) {
    char *dot=(char *)calloc(0, 20);
//    const char *dot = strrchr(filename, '.');
    dot=strrchr(filename, '.');
    if(!dot || dot == filename) *extension="";
    *extension=dot+1;
}

void getTypeFromExtension(char *extension, char **content_type, int *content_type_size, bool *isBinary){
    *isBinary=false;
    if((strcmp(extension, "txt")==0) || (strcmp(extension, "sed")==0) || (strcmp(extension, "awk")==0) || (strcmp(extension, "c")==0) || (strcmp(extension, "h")==0)){
        *content_type=(char *)malloc(11);
        *content_type="text/plain";
        *content_type_size=10;
    }
    else if((strcmp(extension, "html")==0) || (strcmp(extension, "htm")==0)){
        *content_type=(char *)malloc(10);
        *content_type="text/html";
        *content_type_size=9;
    }
    else if((strcmp(extension, "jpeg")==0) || (strcmp(extension, "jpg")==0)){
        *content_type=(char *)malloc(11);
        *content_type="image/jpeg";
        *content_type_size=10;
        *isBinary=true;
    }
    else if(strcmp(extension, "gif")==0){
        *content_type=(char *)malloc(10);
        *content_type="image/gif";
        *content_type_size=9;
        *isBinary=true;
    }
    else if(strcmp(extension, "png")==0){
        *content_type=(char *)malloc(10);
        *content_type="image/png";
        *content_type_size=9;
        *isBinary=true;
    }
    else if(strcmp(extension, "css")==0){
        *content_type=(char *)malloc(9);
        *content_type="text/css";
        *content_type_size=8;
    }
    else if(strcmp(extension, "js")==0){
        *content_type=(char *)malloc(16);
        *content_type="text/javascript";
        *content_type_size=15;
    }
    else if(strcmp(extension, "pdf")==0){
        *content_type=(char *)malloc(16);
        *content_type="application/pdf";
        *content_type_size=15;
        *isBinary=true;
    }
    else{
        *content_type=(char *)malloc(25);
        *content_type="application/octet-stream";
        *content_type_size=24;
        *isBinary=true;
    }
}


int openFileAtLink(char *link, char **extension, bool readContent, unsigned char **content, bool *isBinary, char **content_type, int *content_type_size, unsigned long *content_length){
    char *path=(char *)malloc(6+strlen(link));
//    char *cont=NULL;
//    path="./www";
    strcpy(path, "./www");
    strcat(path, link);
    get_filename_ext(link, extension);
    getTypeFromExtension(*extension, content_type, content_type_size, isBinary);
    
    FILE *fp=NULL;
    if(*isBinary)
        fp=fopen(path, "rb");
    else
        fp=fopen(path, "r");
    if(fp==NULL){
        return FILE_NOT_FOUND;
    }
    
//    int size=512;
//    if(readContent){
//        cont=(char *)malloc(512);
////        *content=(char *)malloc(512);
//        while((fgets(cont, 512, fp))!=NULL){
//            size+=512;
//            char *s=NULL;
//            if((s=(char *)realloc(*content, size))==NULL){
//                return EXIT_FAILURE;
//            }
////            content=(char **)realloc(*content, size);
//            *content=s;
//            strcat(*content, cont);
//            
//            char *u=*content;
//            u+=size-512;
//            memset(u, 'a', 512);
//        }
//    }
    
    
    fseek(fp, 0, SEEK_END);
    *content_length=ftell(fp);
    rewind(fp);
    *content=(unsigned char *)malloc(*content_length);
    fread(*content, *content_length+1, 1, fp);
    
    
    
    free(path);
//    free(cont);
    fclose(fp);
    return FILE_FOUND;
}

int serveRequest(char *request, char **function, char **link, char **protocolName, char **protocolVersion){
//    char *request="GET /index.html HTTP/1.1\r\nUser-Agent: My-web-browser\r\nHost: astarti.cs.ucy.ac.cy:30000\r\nConnection: keep-alive\r\n\r\n";
//    char s[100];
//    char function[30], link[30], protocolName[30], protocolVersion[30];
    char *s=(char *)malloc(1024);
    
    char *token;
    bool keepAlive=true, b=true;
    
    while (sgets(s, 1024, &request) != 0){
        if(b){
            sscanf(s, "%s %s %s\r\n", *function, *link, *protocolName);
            token = strtok(*protocolName, "/");
            sprintf(*protocolName, "%s", token);
            token = strtok(NULL, "/");
            sprintf(*protocolVersion, "%s", token);
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
    
//    printf("%s\n%s\n%s\n%s\n%d\n", *function, *link, *protocolName, *protocolVersion, keepAlive);
    free(s);
    return keepAlive;
}


int readResponseAndExecute(THREAD_NODE *n){//, char **function, char **link, char **protocolName, char **protocolVersion){
    char *buf=(char *)malloc(1024);
    char *function=(char *)malloc(30);
    char *link=(char *)malloc(300);
    char *protocolName=(char *)malloc(30);
    char *protocolVersion=(char *)malloc(30);
    bool keepAlive=true, isBinary=false;
    /* Get message */
    if (read(n->newsock, buf, 1024) < 0) {
        perror("read");
//        exit(1);
    }
    keepAlive=serveRequest(buf, &function, &link, &protocolName, &protocolVersion);
    unsigned char *content=NULL;
    char *response=NULL;
    char *extension=NULL, *content_type=NULL;
    int found;
    unsigned long content_length=0, response_size=0;
    int content_type_length=0;
    char *connection=NULL;
//    if(keepAlive){
//        connection=(char *)malloc(11);
//        connection="keep-alive";
//    }
//    else{
//        connection=(char *)malloc(6);
//        connection="close";
//    }
    connection=(char *)malloc(6);
    connection="close";
    
    if(strcmp(function, "GET")==0){
        //GET function
        
        if((found=openFileAtLink(link, &extension, true, &content, &isBinary, &content_type, &content_type_length, &content_length))==FILE_NOT_FOUND){
            //response as 404-File Not Found
//            response_size=126+19;
//            if(keepAlive)
//                response_size+=10;
//            else
//                response_size+=5;
            response_size=150;
            response=(char *)malloc(response_size);
//            sprintf(response, "HTTP/1.1 404 Not Found\r\nServer: Kaloonda\r\nContent-Length: 20\r\nConnection: %s\r\nContent-Type: text/plain\r\n\r\nDocument not found!\r\n", connection);
            sprintf(response, "HTTP/1.1 404 Not Found\r\nServer: Kaloonda\r\nContent-Length: 20\r\nConnection: close\r\nContent-Type: text/plain\r\n\r\nDocument not found!\r\n");
            /* Get message */
            if (write(n->newsock, response, response_size) < 0) {
                perror("write");
                exit(1);
            }
            
            keepAlive=false;
        }
        if(content==NULL){
            //Unable to realloc. Handle it!
        }
        else{
//            content_length=strlen(content);
            
            char contLengthAsString[10];
            sprintf(contLengthAsString, "%lu", content_length);
//            itoa(content_length, contLengthAsString, 10);
            unsigned long contLengthAsStringLength=strlen(contLengthAsString);
            
            
//            getTypeFromExtension(extension, &content_type, &content_type_length);
            
            
//            header_size=83+content_length+content_type_length;
            response_size=87+content_length+content_type_length+contLengthAsStringLength;
            if(keepAlive)
                response_size+=10;
            else
                response_size+=5;
            response=(char *)malloc(response_size+100);
            if(!isBinary){
                sprintf(response, "HTTP/1.1 200 OK\r\nServer: Kaloonda\r\nContent-Length: %s\r\nConnection: %s\r\nContent-Type: %s\r\n\r\n%s\r\n", contLengthAsString, connection, content_type, content);
                /* Get message */
                if (write(n->newsock, response, response_size) < 0) {
                    perror("write");
                    exit(1);
                }
            }
            else{
                sprintf(response, "HTTP/1.1 200 OK\r\nServer: Kaloonda\r\nContent-Length: %s\r\nConnection: %s\r\nContent-Type: %s\r\n\r\n", contLengthAsString, connection, content_type);
                /* Get message */
                if (write(n->newsock, response, response_size-content_length-2) < 0) {
                    perror("write");
                    exit(1);
                }
                if (send(n->newsock, content, content_length, 0) < 0) {
                    perror("send");
                    exit(1);
                }
            }
//            free(content_type);
        }
        

    }
    else if(strcmp(function, "HEAD")==0){
        //HEAD function
    }
    else if(strcmp(function, "DELETE")==0){
        //DELETE function
    }
    else{
        return false;
    }
    
    
    free(buf);
    free(function);
    free(link);
    free(protocolName);
    free(protocolVersion);
    free(content);
    free(response);
//    free(extension);
    
    
    return keepAlive;
}


void *threadFunction(void *arg){
    int err;
    THREAD_NODE *n=(THREAD_NODE *)arg;
    THREADPOOL *threadpool=NULL;
    returnThreadpool(&threadpool);
    while(1){
        if ((err = pthread_mutex_lock(&(n->nodemutex)))) { /* lock mutex */
            printf("pthread_mutex_lock: %s\n",strerror(err));
            exit(1);
        }
        if((err=pthread_cond_wait(&(n->cond), &(n->nodemutex)))){
            printf("pthread_mutex_lock:%s\n", strerror(err));
            exit(1);
        }
        
        bool keepAlive=true;
//        while(keepAlive){
            keepAlive=readResponseAndExecute(n);//, &function, &link, &protocolName, &protocolVersion);
//        }
        if ((err=pthread_mutex_unlock(&(n->nodemutex)))) { /* unlock mutex */
            printf("pthread_mutex_unlock: %s\n",strerror(err));
            exit(1);
        }
        
        if ((err = pthread_mutex_lock(&(threadpool->poolLock)))) { /* lock mutex */
            printf("pthread_mutex_lock: %s\n",strerror(err));
            exit(1);
        }
        enqueue(threadpool, n);
        if ((err=pthread_mutex_unlock(&(threadpool->poolLock)))) { /* unlock mutex */
            printf("pthread_mutex_unlock: %s\n",strerror(err));
            exit(1);
        }
    }
    
}























