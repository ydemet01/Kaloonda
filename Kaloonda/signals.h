//
//  signals.h
//  Kaloonda
//
//  Created by Yiannis Demetriades on 07/04/17.
//  Copyright © 2017 Yiannis Demetriades - Panayiotis Nicolaou. All rights reserved.
//

#ifndef signals_h
#define signals_h

#define EXIT_ON_ERROR_SYNTAX 90


//Webserver Network-Socket signals
#define SOCKET_DID_NOT_INITIALIZED 10
#define BINDING_FAILED 11
#define LISTEN_FAILED 12
#define ACCEPT_FAILED 13
#define GET_HOST_FAILED 14
#define WEBSERVER_INIT_SUCCESS 18


//Web-requests
#define HTTP_GET_REQUEST 70
#define HTTP_HEAD_REQUEST 71
#define HTTP_DELETE_REQUEST 72
#define HTTP_PUT_REQUEST 73
#define HTTP_NOT_SUPPORTED_REQUEST 79


#define FILE_FOUND 40
#define FILE_NOT_FOUND 41


#endif /* signals_h */
