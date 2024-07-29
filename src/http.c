#include "http.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#define DEBUG 0

void parse_request_line(struct Request *request, char *request_line) {
    const char SP[] = " ";
    char *method = strtok(request_line, SP), *uri = strtok(NULL, SP), *http_version = strtok(NULL, SP);

    if (method == NULL) {
        fprintf(stderr, "Missing method from request...\n");
        free(request);
        exit(EXIT_FAILURE);
    }
    if (uri == NULL) {
        fprintf(stderr, "Missing URI from request...\n");
        free(request);
        exit(EXIT_FAILURE);
    }
    if (http_version == NULL) {
        fprintf(stderr, "Missing HTTP version from request...\n");
        free(request);
        exit(EXIT_FAILURE);
    }

    if (strcmp(method, "GET") == 0) {
        request->method = GET;
    } else if (strcmp(method, "HEAD") == 0) {
        request->method = HEAD;
    } else if (strcmp(method, "POST") == 0) {
        request->method = POST;
    } else {
        fprintf(stderr, "Unrecognized method from request...\n");
        free(request);
        exit(EXIT_FAILURE);
    }

    request->uri = malloc(strlen(uri) + 1);
    strcpy(request->uri, uri);
    request->http_version = malloc(strlen(http_version) + 1);
    strcpy(request->http_version , http_version);
}

void parse_request(struct Request *request, char *request_string) {
    const char CRLF[] = "\r\n", *start = request_string;
    int n = strlen(request_string), i = 0;
    char buffer[256];

    for (char *p = strstr(request_string, CRLF); p != NULL; p = strstr(request_string, CRLF), i++) {
        memset(buffer, 0, 256);
        strncpy(buffer, request_string, p - request_string);
        buffer[p - request_string] = 0;

        if (i == 0) {
            parse_request_line(request, buffer);
        }

        request_string = p + strlen(CRLF);

        if (request_string - start >= n) {
            break;
        }
    }

    char *body = request_string;
    request->body = malloc(strlen(body) + 1);
    strcpy(request->body, body);
}

void handle_request(int client_socket, struct Request *request) {
    int rec_bytes, sent_bytes;
    char buffer[4096] = {0};

    rec_bytes = recv(client_socket, buffer, 512, 0);
    if (rec_bytes <= 0) {
        perror("Unable to read bytes from request...");
        exit(EXIT_FAILURE);
    }

#if DEBUG
    buffer[rec_bytes] = 0;
    for (int i = 0; i < 512; i++) {
        const char LF = '\n', CR = '\r';
        if (buffer[i] == CR) {
            buffer[i] = 'r';
        }
        if (buffer[i] == LF) {
            buffer[i] = 'n';
        }
    }
    puts(buffer);
#endif

    parse_request(request, buffer);
}
