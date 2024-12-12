#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "http-parse.h"
#include "picohttpparser/picohttpparser.h"

// Code in this function is using picohttpparser library. The way of parsing
// with it is fully described in README on github
HTTP_PARSE parse_http_request(int client_sockfd, char* client_request,
                              const int request_size, char* ret_method,
                              const int method_size, char* ret_url,
                              const int url_size, int* minor_version,
                              size_t* buflen) {
    const char* method;
    const char* url;
    struct phr_header headers[100];
    size_t prevbuflen = 0, num_headers, method_len, url_len;
    int pret;

    ssize_t rret;

    while (1) {
        while ((rret = read(client_sockfd, client_request + *buflen,
                            request_size - *buflen)) == -1 &&
               errno == EINTR)
            ;
        if (rret <= 0) {
            printf("Closing. {rret <= 0}\n");
            return PARSE_ERROR;
        }

        prevbuflen = *buflen;
        *buflen += rret;

        num_headers = sizeof(headers) / sizeof(headers[0]);
        pret = phr_parse_request(client_request, *buflen, &method, &method_len,
                                 &url, &url_len, minor_version, headers,
                                 &num_headers, prevbuflen);

        if (pret > 0) {
            break;
        } else if (pret == -1) {
            printf("Closing. {pret == -1}\n");
            return PARSE_ERROR;
        }

        if (pret != -2) {
            printf("Something went wrong during parsing http\n");
            return PARSE_ERROR;
        }

        if (*buflen == request_size) {
            printf("Closing connection. Request is too long\n");
            return PARSE_ERROR;
        }
    }

    if (url_len > url_size) {
        printf("Closing. URL is too long\n");
        return PARSE_ERROR;
    }

    strncpy(ret_method, method, method_size - 1);
    ret_method[method_size - 1] = '\0';

    strcpy(ret_url, url);
    ret_url[url_len - 1] = '\0';

    return PARSE_SUCCESS;
}

HTTP_PARSE parse_http_response(int server_sockfd, char* response,
                               const int response_size, size_t* buflen) {
    const char* msg;
    struct phr_header headers[100];
    size_t prevbuflen = 0, msg_len, num_headers;
    int minor_version, status, pret;
    ssize_t rret;

    while (1) {
        while ((rret = read(server_sockfd, response + *buflen,
                            response_size - *buflen)) == -1 &&
               errno == EINTR)
            ;

        if (rret <= 0) {
            return PARSE_ERROR;
        }

        prevbuflen = *buflen;
        *buflen += rret;

        num_headers = sizeof(headers) / sizeof(headers[0]);

        pret =
            phr_parse_response(response, *buflen, &minor_version, &status, &msg,
                               &msg_len, headers, &num_headers, prevbuflen);

        if (pret > 0) {
            break;
        } else if (pret == -1) {
            printf("Closing. {pret == -1}\n");
            return PARSE_ERROR;
        }

        if (pret != -2) {
            printf("Something went wrong during parsing http\n");
            return PARSE_ERROR;
        }

        if (*buflen == response_size) {
            printf("Closing connection. Request is too long\n");
            return PARSE_ERROR;
        }
    }

    return PARSE_SUCCESS;
}
