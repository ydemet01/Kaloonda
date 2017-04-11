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

void get_filename_ext(char *filename, char **extension) {
    char *dot=(char *)calloc(0, 20);
    dot=strrchr(filename, '.');
    if(!dot || dot == filename) *extension="";
    *extension=dot+1;
}

void getTypeFromExtension(char *extension, char **content_type, int *content_type_size, bool *isBinary){
    if(isBinary!=NULL)
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
        if(isBinary!=NULL)
            *isBinary=false;
    }
    else if(strcmp(extension, "gif")==0){
        *content_type=(char *)malloc(10);
        *content_type="image/gif";
        *content_type_size=9;
        if(isBinary!=NULL)
            *isBinary=false;
    }
    else if(strcmp(extension, "png")==0){
        *content_type=(char *)malloc(10);
        *content_type="image/png";
        *content_type_size=9;
        if(isBinary!=NULL)
            *isBinary=false;
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
        if(isBinary!=NULL)
            *isBinary=false;
    }
    else{
        *content_type=(char *)malloc(25);
        *content_type="application/octet-stream";
        *content_type_size=24;
        if(isBinary!=NULL)
            *isBinary=false;
    }
}


int openFileAtLink(char *link, char **extension, bool readContent, unsigned char **content, bool *isBinary, char **content_type, int *content_type_size, unsigned long *content_length){
    char *path=(char *)malloc(6+strlen(link));
    strcpy(path, "./www");
    strcat(path, link);
    get_filename_ext(link, extension);
    getTypeFromExtension(*extension, content_type, content_type_size, isBinary);
    
    FILE *fp=NULL;
    if(isBinary!=NULL && *isBinary)
        fp=fopen(path, "rb");
    else
        fp=fopen(path, "r");
    if(fp==NULL){
        return FILE_NOT_FOUND;
    }
    
    fseek(fp, 0, SEEK_END);
    *content_length=ftell(fp);
    rewind(fp);
    
    if(!readContent){
        free(path);
        fclose(fp);
        return FILE_FOUND;
    }
    
    
    *content=(unsigned char *)malloc(*content_length);
    fread(*content, *content_length+1, 1, fp);
    
    free(path);
    fclose(fp);
    return FILE_FOUND;
}

int serveRequest(char *request, char **function, char **link, char **protocolName, char **protocolVersion){
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
    free(s);
    return keepAlive;
}

void createHeaderAndReturnContent(char **header, int webRequestFlag, unsigned char **content, char *link, bool *isBinary, unsigned long *content_length){
    char *extension=NULL, *content_type=NULL;
    int found;
    int content_type_length=0;
    unsigned long header_size;
    if(webRequestFlag==HTTP_GET_REQUEST)
        found=openFileAtLink(link, &extension, true, content, isBinary, &content_type, &content_type_length, content_length);
    else if(webRequestFlag==HTTP_HEAD_REQUEST)
        found=openFileAtLink(link, &extension, false, NULL, NULL, &content_type, &content_type_length, content_length);
    else if(webRequestFlag==HTTP_DELETE_REQUEST){
        found=openFileAtLink(link, &extension, false, NULL, NULL, &content_type, &content_type_length, content_length);
    }
    else{
        header_size=114;
        *content=(unsigned char *)"<html><body><h1>Method not implemented.</h1></body></html>";
        *header=(char *) malloc(header_size);
        sprintf(*header, "HTTP/1.1 501 Not Implemented\r\nServer: Kaloonda\r\nContent-Length: 50\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n");
        return;
    }
    
    if(found==FILE_NOT_FOUND){
        //404 - File Not Found
        unsigned long header_size=113;
        *header=(char *) malloc(header_size);
        *content=(unsigned char *)"<html><body><h1>File not found.</h1></body></html>";
        sprintf(*header, "HTTP/1.1 404 File Not Found\r\nServer: Kaloonda\r\nContent-Length: 50\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n");
    }
    else{
        char contLengthAsString[10];
        sprintf(contLengthAsString, "%lu", *content_length);
        unsigned long contLengthAsStringLength=strlen(contLengthAsString);
        if(webRequestFlag==HTTP_GET_REQUEST || webRequestFlag==HTTP_HEAD_REQUEST){
            header_size=90+content_type_length+contLengthAsStringLength;
            
            *header=(char *) malloc(header_size);
            sprintf(*header, "HTTP/1.1 200 OK\r\nServer: Kaloonda\r\nContent-Length: %s\r\nConnection: close\r\nContent-Type: %s\r\n\r\n", contLengthAsString, content_type);
        }
        else if(webRequestFlag==HTTP_DELETE_REQUEST){
            int delRes;
            char *path=(char *)malloc(6+strlen(link));
            strcpy(path, "./www");
            strcat(path, link);
            delRes=remove(path);
            //            free(content);
            if(!delRes){
                header_size=99+contLengthAsStringLength;
                *content=(unsigned char *)"<html><body><h1>URL deleted.</h1></body></html>";
                *header=(char *) malloc(header_size);
                sprintf(*header, "HTTP/1.1 200 OK\r\nServer: Kaloonda\r\nContent-Length: %s\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n", contLengthAsString);
            }
            else{
                header_size=118+contLengthAsStringLength;
                *content=(unsigned char *)"<html><body><h1>File was not deleted.</h1></body></html>";
                *header=(char *) malloc(header_size);
                sprintf(*header, "HTTP/1.1 500 Internal Server Error\r\nServer: Kaloonda\r\nContent-Length: %s\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n", contLengthAsString);
            }
        }
    }
}


int readResponseAndExecute(THREAD_NODE *n){
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
    char *header=NULL;
    unsigned long content_length=0;
    
    if(strcmp(function, "GET")==0){
        //GET function
        createHeaderAndReturnContent(&header, HTTP_GET_REQUEST, &content, link, &isBinary, &content_length);
        if (write(n->newsock, header, strlen(header)) < 0) {
            perror("write");
            exit(1);
        }
        if(!isBinary){
            if (write(n->newsock, content, strlen((char *)content)) < 0) {
                perror("write");
                exit(1);
            }
            if (write(n->newsock, "\r\n", 2) < 0) {
                perror("write");
                exit(1);
            }
        }
        else{
            if (send(n->newsock, content, strlen((char *)content), 0) < 0) {
                perror("send");
                exit(1);
            }
        }
        free(content);
        
    }
    else if(strcmp(function, "HEAD")==0){
        //HEAD function
        createHeaderAndReturnContent(&header, HTTP_HEAD_REQUEST, NULL, link, NULL, &content_length);
        if (write(n->newsock, header, strlen(header)) < 0) {
            perror("write");
            exit(1);
        }
    }
    else if(strcmp(function, "DELETE")==0){
        //DELETE function
        createHeaderAndReturnContent(&header, HTTP_DELETE_REQUEST, &content, link, NULL, &content_length);
        if (write(n->newsock, header, strlen(header)) < 0) {
            perror("write");
            exit(1);
        }
        if (write(n->newsock, content, strlen((char *)content)) < 0) {
            perror("write");
            exit(1);
        }
        if (write(n->newsock, "\r\n", 2) < 0) {
            perror("write");
            exit(1);
        }
    }
    else{
        createHeaderAndReturnContent(&header, HTTP_NOT_SUPPORTED_REQUEST, &content, NULL, NULL, NULL);
        if (write(n->newsock, header, strlen(header)) < 0) {
            perror("write");
            exit(1);
        }
        if (write(n->newsock, content, strlen((char *)content)) < 0) {
            perror("write");
            exit(1);
        }
        if (write(n->newsock, "\r\n", 2) < 0) {
            perror("write");
            exit(1);
        }
    }
    
    
    free(buf);
    free(function);
    free(link);
    free(protocolName);
    free(protocolVersion);
    free(header);
    
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























