#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int parse_request_line(struct Request *request, char *request_line) {
    char *method = strtok(request_line, SP), *uri = strtok(NULL, SP), *http_version = strtok(NULL, SP);

    if (method == NULL) {
        fprintf(stderr, "Missing method from request...\n");
        free(request->request_line), request->request_line = NULL;
        free(request), request = NULL;
        return -1;
    }
    if (uri == NULL) {
        fprintf(stderr, "Missing URI from request...\n");
        free(request->request_line), request->request_line = NULL;
        free(request), request = NULL;
        return -1;
    }
    if (http_version == NULL) {
        fprintf(stderr, "Missing HTTP version from request...\n");
        free(request->request_line), request->request_line = NULL;
        free(request), request = NULL;
        return -1;
    }

    if (strcmp(method, "GET") == 0) {
        request->request_line->method = GET;
    } else if (strcmp(method, "HEAD") == 0) {
        request->request_line->method = HEAD;
    } else if (strcmp(method, "POST") == 0) {
        request->request_line->method = POST;
    } else {
        fprintf(stderr, "Unrecognized method '%s'...\n", method);
        free(request->request_line), request->request_line = NULL;
        free(request), request = NULL;
        return -1;
    }

    request->request_line->uri = malloc(strlen(uri) + 1);
    if (request->request_line->uri == NULL) {
        fprintf(stderr, "Failed to allocate memory for request line URI...\n");
        free(request->request_line), request->request_line = NULL;
        free(request), request = NULL;
        return -1;
    }
    strcpy(request->request_line->uri, uri);

    request->request_line->http_version = malloc(strlen(http_version) + 1);
    if (request->request_line->http_version == NULL) {
        fprintf(stderr, "Failed to allocate memory for request line HTTP version...\n");
        free(request->request_line->uri), request->request_line->uri = NULL;
        free(request->request_line), request->request_line = NULL;
        free(request), request = NULL;
        return -1;
    }
    strcpy(request->request_line->http_version, http_version);

    return 0;
}

int parse_request(struct Request *request, char *request_string) {
    const char *start = request_string;
    int n = strlen(request_string), i = 0;
    char buffer[256] = {0};

    // Parses request line and headers
    for (char *p = strstr(request_string, CRLF); p != NULL; p = strstr(request_string, CRLF), i++) {
        memset(buffer, 0, 256);
        strncpy(buffer, request_string, p - request_string);
        buffer[p - request_string] = 0;

        if (i == 0) {
            if (parse_request_line(request, buffer) < 0) {
                return -1;
            }
        }

        request_string = p + strlen(CRLF);
        if (request_string - start >= n) {
            break;
        }
    }

    // Parses request body
    char *body = request_string;
    request->body = malloc(strlen(body) + 1);
    if (request->body == NULL) {
        fprintf(stderr, "Failed to allocate memory for request body...\n");
        free(request->request_line->http_version), request->request_line->http_version = NULL;
        free(request->request_line->uri), request->request_line->uri = NULL;
        free(request->request_line), request->request_line = NULL;
        free(request), request = NULL;
        return -1;
    }
    strcpy(request->body, body);

    return 0;
}
