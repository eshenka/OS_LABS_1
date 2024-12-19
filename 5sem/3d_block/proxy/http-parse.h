#ifndef HTTP_PARSE_H
#define HTTP_PARSE_H

#include "list.h"
#include <string.h>

typedef enum HTTP_PARSE {
    PARSE_SUCCESS,
    PARSE_ERROR,
} HTTP_PARSE;

HTTP_PARSE parse_http_request(int client_sockfd, char* client_request,
                              int request_size, char* ret_method,
                              const int method_size, char* ret_url,
                              const int url_size, int* minor_version,
                              size_t* buflen);

HTTP_PARSE parse_http_response(int server_sockfd, List* response,
                               int reponse_size, size_t* buflen);

#endif // !HTTP_PARSE_H
