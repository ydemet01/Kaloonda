//
//  httprequest.c
//  Kaloonda
//
//  Created by Yiannis Demetriades on 08/04/17.
//  Copyright © 2017 Yiannis Demetriades - Panayiotis Nicolaou. All rights reserved.
//

#include "httprequest.h"

unsigned char *sgets( unsigned char * str, int num, unsigned char **input, bool ignoreNull )
{
    unsigned char *next = *input;
    int  numread = 0;
    bool b=false;
    if(ignoreNull)
        b=true;
    else
        b=*next;
    
    while ( numread + 1 < num && b ) {
        int isnewline = ( *next == '\n' && b );
        *str++ = *next++;
        numread++;
        // newline terminates the line but is included
        if ( isnewline )
            break;
        if(ignoreNull)
            b=true;
        else
            b=*next;
    }
    
    if ( numread == 0 )
        return NULL;  // "eof"
    
    // must have hit the null terminator or end of line
    if(!b)
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
            *isBinary=true;
    }
    else if(strcmp(extension, "gif")==0){
        *content_type=(char *)malloc(10);
        *content_type="image/gif";
        *content_type_size=9;
        if(isBinary!=NULL)
            *isBinary=true;
    }
    else if(strcmp(extension, "png")==0){
        *content_type=(char *)malloc(10);
        *content_type="image/png";
        *content_type_size=9;
        if(isBinary!=NULL)
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
        if(isBinary!=NULL)
            *isBinary=true;
    }
    else{
        *content_type=(char *)malloc(25);
        *content_type="application/octet-stream";
        *content_type_size=24;
        if(isBinary!=NULL)
            *isBinary=true;
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
    *content=(unsigned char *)malloc(*content_length+1);
    fread(*content, 1, *content_length+1, fp);
    
    free(path);
    fclose(fp);
    return FILE_FOUND;
}

void checkBinary(unsigned char *type, bool *isBinary){
    if((strcmp((char *)type,"image/jpeg")==0)||(strcmp((char *)type,"image/gif")==0)||(strcmp((char *)type,"text/css")==0)||(strcmp((char *)type,"image/png")==0)||(strcmp((char *)type,"text/javascript")==0) ||(strcmp((char *)type,"application/pdf")==0)||(strcmp((char *)type,"application/octet-stream")==0))
        *isBinary=true;
    else *isBinary=false;
}

int serveRequest(unsigned char *request, char **function, char **link, char **protocolName, char **protocolVersion, unsigned char **body, unsigned long *bodyLength){
    unsigned char *s=(unsigned char *)malloc(1024);
    bool isBinaryPUT=false,readBody=false;
    char *token;
    int contentLength=0;
    bool keepAlive=true;
    int b=0;
    bzero(s, 1024);
    while (sgets(s, 1024, &request, false) != 0){
        if(b==0){
            sscanf((char *)s, "%s %s %s\r\n", *function, *link, *protocolName);
            token = strtok(*protocolName, "/");
            sprintf(*protocolName, "%s", token);
            token = strtok(NULL, "/");
            sprintf(*protocolVersion, "%s", token);
            if(strcmp("PUT",*function)==0)
                readBody=true;
            b=1;
        }
        else if(b==1){
            if (strcmp((char *)s,"\r\n")==0 && readBody){
                b=2;
                break;
            }
            unsigned char *t1=(unsigned char *) malloc(500);
            unsigned char *t2=(unsigned char *) malloc(500);
            token = strtok((char *)s, " ");
            sprintf((char *)t1, "%s", token);
            token = strtok(NULL, "\r");
            sprintf((char *)t2, "%s", token);
            if(strcmp((char *)t1,"Content-Type:")==0){
                checkBinary(t2,&isBinaryPUT);
            }
            if(strcmp((char *)t1,"Content-Length:")==0){
                contentLength=atoi((char *)t2);
            }
            if(strcmp((char *)t1,"Connection:")==0){
                if(strcmp((char *)t2, "close")==0)
                    keepAlive=false;
            }
            free(t1);
            free(t2);
        }
        bzero(s, 1024);
    }
    if(b==2){
        *bodyLength=contentLength;
        if(!isBinaryPUT)
            contentLength+=1;
        *body=(unsigned char*)malloc(contentLength);
        if(isBinaryPUT){
            unsigned char *u=*body;
            int i=0;
            for(i=0; i<contentLength; i++){
                *u=*request;
                u++;
                request++;
            }
            printf("%d", i);
        }
        else{
            sgets(*body, contentLength, &request, false);
        }
    }
    free(s);
    return keepAlive;
}

void onServerError(char **header, unsigned char **content){
    time_t rawtime;
    struct tm *info;
    time(&rawtime);
    /* Get GMT time */
    info = gmtime(&rawtime );
    char day[4], month[4], year[5], time[9];
    strftime(day, 4, "%a", info);
    strftime(month, 4, "%b", info);
    strftime(year, 5, "%Y", info);
    strftime(time, 9, "%X", info);
    
    int header_size=157;
    *content=(unsigned char *)"<html><body><h1>Internal Server Error.</h1></body></html>";
    *header=(char *) malloc(header_size);
    sprintf(*header, "HTTP/1.1 500 Internal Server Error\r\nDate: %s, %2d %s %s %s GMT\r\nServer: Kaloonda\r\nContent-Length: 57\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n", day, info->tm_mday, month, year, time);
}

void protocolVersionNotSupported(char **header, unsigned char **content){
    time_t rawtime;
    struct tm *info;
    time(&rawtime);
    /* Get GMT time */
    info = gmtime(&rawtime );
    char day[4], month[4], year[5], time[9];
    strftime(day, 4, "%a", info);
    strftime(month, 4, "%b", info);
    strftime(year, 5, "%Y", info);
    strftime(time, 9, "%X", info);
    
    int header_size=163;
    *content=(unsigned char *)"<html><body><h1>HTTP version not supported.</h1></body></html>";
    *header=(char *) malloc(header_size);
    sprintf(*header, "HTTP/1.1 505 HTTP Version Not Supported\r\nDate: %s, %2d %s %s %s GMT\r\nServer: Kaloonda\r\nContent-Length: 57\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n", day, info->tm_mday, month, year, time);
}

void protocolNameNotSupported(char **header, unsigned char **content){
    time_t rawtime;
    struct tm *info;
    time(&rawtime);
    /* Get GMT time */
    info = gmtime(&rawtime );
    char day[4], month[4], year[5], time[9];
    strftime(day, 4, "%a", info);
    strftime(month, 4, "%b", info);
    strftime(year, 5, "%Y", info);
    strftime(time, 9, "%X", info);
    
    int header_size=163;
    *content=(unsigned char *)"<html><body><h1>HTTP protocol not supported.</h1></body></html>";
    *header=(char *) malloc(header_size);
    sprintf(*header, "HTTP/1.1 400 Bad Request\r\nDate: %s, %2d %s %s %s GMT\r\nServer: Kaloonda\r\nContent-Length: 57\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n", day, info->tm_mday, month, year, time);
}


void createHeaderAndReturnContent(char **header, int webRequestFlag, unsigned char **content, char *link, bool *isBinary, unsigned long *content_length,
                                  unsigned char *body, unsigned long bodyLength){
    char *extension=NULL, *content_type=NULL;
    int found;
    int content_type_length=0;
    unsigned long header_size;
    char *path=(char *)malloc(6+strlen(link));
    strcpy(path, "./www");
    strcat(path, link);
    
    time_t rawtime;
    struct tm *info;
    time(&rawtime);
    /* Get GMT time */
    info = gmtime(&rawtime );
    char day[4], month[4], year[5], time[9];
    strftime(day, 4, "%a", info);
    strftime(month, 4, "%b", info);
    strftime(year, 5, "%Y", info);
    strftime(time, 9, "%X", info);
    
    struct stat attr;
    stat(path, &attr);
    time_t lmod=attr.st_mtime;
    struct tm *lmodtm;
    /* Get GMT time */
    lmodtm = gmtime(&lmod );
    char daylm[4], monthlm[4], yearlm[5], timelm[9];
    strftime(daylm, 4, "%a", lmodtm);
    strftime(monthlm, 4, "%b", lmodtm);
    strftime(yearlm, 5, "%Y", lmodtm);
    strftime(timelm, 9, "%X", lmodtm);
    
    if(webRequestFlag==HTTP_GET_REQUEST)
        found=openFileAtLink(link, &extension, true, content, isBinary, &content_type, &content_type_length, content_length);
    else if(webRequestFlag==HTTP_HEAD_REQUEST)
        found=openFileAtLink(link, &extension, false, NULL, NULL, &content_type, &content_type_length, content_length);
    else if(webRequestFlag==HTTP_DELETE_REQUEST){
        found=openFileAtLink(link, &extension, false, NULL, NULL, &content_type, &content_type_length, content_length);
    }
    else if(webRequestFlag==HTTP_NOT_SUPPORTED_REQUEST){
        header_size=151;
        *content=(unsigned char *)"<html><body><h1>Method not implemented.</h1></body></html>";
        *header=(char *) malloc(header_size);
        sprintf(*header, "HTTP/1.1 501 Not Implemented\r\nDate: %s, %2d %s %s %s GMT\r\nServer: Kaloonda\r\nContent-Length: 80\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n", day, info->tm_mday, month, year, time);
        return;
    }
    
    if(found==FILE_NOT_FOUND){
        //404 - File Not Found
        unsigned long header_size=150;
        *header=(char *) malloc(header_size);
        *content=(unsigned char *)"<html><body><h1>File not found.</h1></body></html>";
        sprintf(*header, "HTTP/1.1 404 File Not Found\r\nDate: %s, %2d %s %s %s GMT\r\nServer: Kaloonda\r\nContent-Length: 80\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n", day, info->tm_mday, month, year, time);
    }
    else{
        char contLengthAsString[10];
        sprintf(contLengthAsString, "%lu", *content_length);
        unsigned long contLengthAsStringLength=strlen(contLengthAsString);
        if(webRequestFlag==HTTP_GET_REQUEST || webRequestFlag==HTTP_HEAD_REQUEST){
            header_size=200+content_type_length+contLengthAsStringLength;
            
            *header=(char *) malloc(header_size);
            sprintf(*header, "HTTP/1.1 200 OK\r\nDate: %s, %2d %s %s %s GMT\r\nServer: Kaloonda\r\nLast-Modified: %s, %2d %s %s %s GMT\r\nContent-Length: %s\r\nConnection: close\r\nContent-Type: %s\r\n\r\n", day, info->tm_mday, month, year, time, daylm, lmodtm->tm_mday, monthlm, yearlm, timelm, contLengthAsString, content_type);
        }
        else if(webRequestFlag==HTTP_DELETE_REQUEST){
            int delRes;
            char *path=(char *)malloc(6+strlen(link));
            strcpy(path, "./www");
            strcat(path, link);
            delRes=remove(path);
            if(!delRes){
                header_size=138;
                *content=(unsigned char *)"<html><body><h1>URL deleted.</h1></body></html>";
                *header=(char *) malloc(header_size);
                sprintf(*header, "HTTP/1.1 200 OK\r\nDate: %s, %2d %s %s %s GMT\r\nServer: Kaloonda\r\nContent-Length: 48\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n", day, info->tm_mday, month, year, time);
            }
            else{
                header_size=157;
                *content=(unsigned char *)"<html><body><h1>File was not deleted.</h1></body></html>";
                *header=(char *) malloc(header_size);
                sprintf(*header, "HTTP/1.1 500 Internal Server Error\r\nDate: %s, %2d %s %s %s GMT\r\nServer: Kaloonda\r\nContent-Length: 57\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n", day, info->tm_mday, month, year, time);
                return;
            }
        }
        else if (webRequestFlag==HTTP_PUT_REQUEST){
            FILE* newFile=fopen(path,"wb");
            if (newFile==NULL){
                onServerError(header, content);
                return;
            }
            unsigned long writen=fwrite(body, 1, bodyLength,newFile);
            fclose(newFile);
            if (writen!=bodyLength) {
                onServerError(header, content);
                return;
            }
            header_size=143;
            *content=(unsigned char *)"<html><body><h1>The File was created.</h1></body></html>";
            *header=(char *) malloc(header_size);
            sprintf(*header, "HTTP/1.1 201 Created\r\nDate: %s, %2d %s %s %s GMT\r\nServer: Kaloonda\r\nContent-Length: 56\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n", day, info->tm_mday, month, year, time);
        }
    }
}

int readResponseAndExecute(THREAD_NODE *n){
    unsigned char *buf=(unsigned char *)malloc(50000);
    char *function=(char *)malloc(30);
    char *link=(char *)malloc(300);
    char *protocolName=(char *)malloc(30);
    char *protocolVersion=(char *)malloc(30);
    bool keepAlive=true, isBinary=false;
    unsigned char *body=NULL;
    unsigned long bodyLength=0;
    unsigned char *content=NULL;
    char *header=NULL;
    unsigned long content_length=0;
    
    if(recv(n->newsock, (unsigned char *)buf, 50000, 0)<0){
        perror("recv");
        exit(1);
    }
    
    keepAlive=serveRequest(buf, &function, &link, &protocolName, &protocolVersion, &body, &bodyLength);
    if(strcmp(protocolName, "HTTP")!=0){
        protocolNameNotSupported(&header, &content);
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
        return false;
    }
    if(strcmp(protocolVersion, "1.1")!=0){
        protocolVersionNotSupported(&header, &content);
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
        return false;
    }
    
    
    
    if(strcmp(function, "GET")==0){
        //GET function
        createHeaderAndReturnContent(&header, HTTP_GET_REQUEST, &content, link, &isBinary, &content_length, NULL, 0);
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
            if (send(n->newsock, content, content_length, 0) < 0) {
                perror("send");
                exit(1);
            }
        }
        
    }
    else if(strcmp(function, "HEAD")==0){
        //HEAD function
        createHeaderAndReturnContent(&header, HTTP_HEAD_REQUEST, NULL, link, NULL, &content_length, NULL, 0);
        if (write(n->newsock, header, strlen(header)) < 0) {
            perror("write");
            exit(1);
        }
    }
    else if(strcmp(function, "DELETE")==0){
        //DELETE function
        createHeaderAndReturnContent(&header, HTTP_DELETE_REQUEST, &content, link, NULL, &content_length, NULL, 0);
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
    else if(strcmp(function, "PUT") == 0) {
        createHeaderAndReturnContent(&header, HTTP_PUT_REQUEST, &content, link, NULL, &content_length,body, bodyLength);
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
        createHeaderAndReturnContent(&header, HTTP_NOT_SUPPORTED_REQUEST, &content, NULL, NULL, NULL, NULL, 0);
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
        keepAlive=readResponseAndExecute(n);
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

#ifdef HTTP_REQUEST_TEST
int main(){
    unsigned char *buf=NULL;
    char *function=(char *)malloc(30);
    char *link=(char *)malloc(300);
    char *protocolName=(char *)malloc(30);
    char *protocolVersion=(char *)malloc(30);
    bool keepAlive=true, isBinary=false;
    unsigned char *body=NULL;
    unsigned long bodyLength=0;
    unsigned char *content=NULL;
    char *header=NULL;
    unsigned long content_length=0;
    
    int i=0;
    
    for(i=0; i<6; i++){
        buf=(unsigned char *)malloc(1000);
        
        switch (i) {
            case 0:
                strcpy(buf, "GET /epl371/index.html HTTPF/1.1\r\n\r\n");
                break;
            case 1:
                strcpy(buf, "GET /epl371/index.html HTTP/1.0\r\n\r\n");
                break;
            case 2:
                strcpy(buf, "GET /epl371/index.html HTTP/1.1\r\n\r\n");
                break;
            case 3:
                strcpy(buf, "HEAD /epl371/index.html HTTP/1.1\r\n\r\n");
                break;
            case 4:
                strcpy(buf, "DELETE /epl371/file.txt HTTP/1.1\r\n\r\n");
                break;
            case 5:
                strcpy(buf, "OPTIONS /epl371/file.txt HTTP/1.1\r\n\r\n");
                break;
            default:
                strcpy(buf, "GET /epl371/index.html HTTPF/1.1\r\n\r\n");
                break;
        }
        
        serveRequest(buf, &function, &link, &protocolName, &protocolVersion, &body, &bodyLength);
        
        if(strcmp(protocolName, "HTTP")!=0){
            if(i==0){
                printf("TEST 1 Successful : Protocol Test");
            }
            else{
                printf("TEST 1 Failure : Protocol Test");
            }
            continue;
        }
        if(strcmp(protocolVersion, "1.1")!=0){
            if(i==1){
                printf("TEST 2 Successful : Protocol Version Test");
            }
            else{
                printf("TEST 2 Failure : Protocol Version Test");
            }
            continue;
        }
        
        
        
        if(strcmp(function, "GET")==0){
            //GET function
            createHeaderAndReturnContent(&header, HTTP_GET_REQUEST, &content, link, &isBinary, &content_length, NULL, 0);
            if(i==2){
                printf("TEST 3 Successful : GET Request Test");
            }
            else{
                printf("TEST 3 Failure : GET Request Test");
            }
            continue;
            
        }
        else if(strcmp(function, "HEAD")==0){
            //HEAD function
            createHeaderAndReturnContent(&header, HTTP_HEAD_REQUEST, NULL, link, NULL, &content_length, NULL, 0);
            if(i==3){
                printf("TEST 4 Successful : HEAD Request Test");
            }
            else{
                printf("TEST 4 Failure : HEAD Request Test");
            }
            continue;
        }
        else if(strcmp(function, "DELETE")==0){
            //DELETE function
            createHeaderAndReturnContent(&header, HTTP_DELETE_REQUEST, &content, link, NULL, &content_length, NULL, 0);
            if(i==4){
                printf("TEST 5 Successful : DELETE Request Test");
            }
            else{
                printf("TEST 5 Failure : DELETE Request Test");
            }
            continue;
        }
        else{
            createHeaderAndReturnContent(&header, HTTP_NOT_SUPPORTED_REQUEST, &content, NULL, NULL, NULL, NULL, 0);
            if(i==5){
                printf("TEST 6 Successful : DELETE Request Test");
            }
            else{
                printf("TEST 6 Failure : DELETE Request Test");
            }
        }
        
        free(buf)
        free(function);
        free(link);
        free(protocolName);
        free(protocolVersion);
        free(header);
        
    }
}
#endif























