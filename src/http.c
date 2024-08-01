#include "http.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

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
        request->request_line->method = GET;
    } else if (strcmp(method, "HEAD") == 0) {
        request->request_line->method = HEAD;
    } else if (strcmp(method, "POST") == 0) {
        request->request_line->method = POST;
    } else {
        fprintf(stderr, "Unrecognized method from request...\n");
        free(request);
        exit(EXIT_FAILURE);
    }

    request->request_line->uri = malloc(strlen(uri) + 1);
    strcpy(request->request_line->uri, uri);
    request->request_line->http_version = malloc(strlen(http_version) + 1);
    strcpy(request->request_line->http_version , http_version);
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
    int rec_bytes;
    char buffer[4096] = {0};

    rec_bytes = recv(client_socket, buffer, 4096, 0);
    if (rec_bytes <= 0) {
        perror("Unable to read bytes from request...");
        exit(EXIT_FAILURE);
    }

    parse_request(request, buffer);
}
