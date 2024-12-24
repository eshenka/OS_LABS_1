#include <errno.h>
#include <malloc.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "cache.h"
#include "http-parse.h"
#include "list.h"
#include "picohttpparser/picohttpparser.h"

// Code in this function is using picohttpparser library. The way of parsing
// with it is fully described in README on github
HTTP_PARSE http_parse_read_request(int client_sockfd, char* client_request,
                                   int request_size, char* ret_method,
                                   const int method_size, char* ret_url,
                                   const int url_size, const int max_url_size,
                                   int* minor_version, size_t* buflen) {
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
            printf("[ERROR] Error reading from client\n");
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
        }

        if (pret != -2) {
            printf("[ERROR] Error parsing client request\n");
            return PARSE_ERROR;
        }

        if (*buflen == request_size) {
            printf("[ERROR] Client request is too long\n");
            return PARSE_ERROR;
        }
    }

    if (url_len > max_url_size) {
        printf("[ERROR] URL is too long\n");
        return PARSE_ERROR;
    }

    if (url_len > url_size) {
        ret_url = realloc(ret_url, sizeof(char) * url_len);
    }

    memcpy(ret_method, method, method_size - 1);
    ret_method[method_size - 1] = '\0';

    memcpy(ret_url, url, url_len);
    ret_url[url_len] = '\0';

    printf("HTTP GET %s\n", ret_url);

    return PARSE_SUCCESS;
}

HTTP_PARSE http_parse_read_response(int server_sockfd, int response_size,
                                    size_t* buflen, CacheEntry* entry) {
    const char* msg;
    struct phr_header headers[100];
    size_t prevbuflen = 0, msg_len, num_headers;
    int minor_version, status, pret;
    ssize_t rret;

    size_t chunk_size = response_size;
    size_t chunk_len = *buflen;
    List* node = entry->data;
    List* head = entry->data;

    int content_length = 0;

    char buffer[chunk_size];

    while (1) {
        while ((rret = read(server_sockfd, buffer + chunk_len,
                            chunk_size - chunk_len)) == -1 &&
               errno == EINTR)
            ;

        if (rret <= 0) {
            return PARSE_ERROR;
        }

        prevbuflen = *buflen;
        *buflen += rret;
        chunk_len += rret;

        if (chunk_len == chunk_size) {
            pthread_rwlock_wrlock(&entry->lock);

            memcpy(node->buffer, buffer, chunk_len);
            node->buf_len = chunk_len;
            entry->parts_done += 1;

            list_add_node(node, chunk_size);
            node = node->next;
            pthread_cond_broadcast(&entry->new_part);

            pthread_rwlock_unlock(&entry->lock);
            chunk_len = 0;
        }

        num_headers = sizeof(headers) / sizeof(headers[0]);

        pret =
            phr_parse_response(buffer, *buflen, &minor_version, &status, &msg,
                               &msg_len, headers, &num_headers, prevbuflen);

        if (pret > 0) {
            pthread_rwlock_wrlock(&entry->lock);

            memcpy(node->buffer, buffer, chunk_len);
            node->buf_len = chunk_len;

            pthread_rwlock_unlock(&entry->lock);
            break;
        } else if (pret == -1) {
            printf("Closing. {pret == -1}\n");
            return PARSE_ERROR;
        }

        if (pret != -2) {
            printf("Something went wrong during parsing http\n");
            return PARSE_ERROR;
        }

        if (*buflen >= response_size) {
            printf("Closing connection. Request is too long\n");
            return PARSE_ERROR;
        }
    }

    for (int i = 0; i < num_headers; i++) {
        if (strncmp(headers[i].name, "Content-Length", headers[i].name_len)) {
            continue;
        }

        content_length = atoi(headers[i].value);
    }

    if (content_length == 0) {
        printf("\n\n\nContent length is not provided\n\n\n");
    }

    rret = 0;
    int offset = (content_length + pret) % chunk_size;
    while (*buflen < content_length + pret) {
        rret = read(server_sockfd, buffer + chunk_len, chunk_size - chunk_len);
        if (rret == -1) {
            printf("Read response error\n");
            return PARSE_ERROR;
        }

        *buflen += rret;
        chunk_len += rret;

        if (chunk_len >= chunk_size) {
            pthread_rwlock_wrlock(&entry->lock);

            memcpy(node->buffer, buffer, chunk_len);
            __sync_fetch_and_add(&entry->parts_done, 1);
            pthread_cond_broadcast(&entry->new_part);
            node->buf_len = chunk_len;

            list_add_node(node, chunk_size);
            node = node->next;

            pthread_rwlock_unlock(&entry->lock);

            chunk_len = 0;
            if (content_length + pret - *buflen <= chunk_size) {
                chunk_size = offset;
            }
        }
    }

    pthread_rwlock_wrlock(&entry->lock);
    entry->done = true;
    pthread_rwlock_unlock(&entry->lock);
    pthread_cond_broadcast(&entry->new_part);

    return PARSE_SUCCESS;
}
