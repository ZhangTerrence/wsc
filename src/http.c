#include "http.h"
#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#define DEBUG 1

char *get_method_string(enum RequestMethod method) {
    switch (method) {
        case 0:
            return "GET";
        case 1:
            return "HEAD";
        case 2:
            return "POST";
        default:
            return NULL;
    }
}

char *get_reason_phrase_string(int status_code) {
    switch (status_code) {
        case 200:
            return "OK";
        case 400:
            return "Bad Request";
        default:
            return NULL;
    }
}

int handle_request(int client_socket, struct Request *request) {
    int rec_bytes;
    char buffer[4096] = {0};

    rec_bytes = recv(client_socket, buffer, 4096, 0);
    if (rec_bytes <= 0) {
        fprintf(stderr, "Failed to read bytes from request...\n");
        free(request->request_line), free(request);
        request->request_line = NULL, request = NULL;
        return -1;
    }

    return parse_request(request, buffer);
}

struct Response *create_response(int client_socket) {
    struct Response *response = malloc(sizeof(struct Response));
    response->client_socket = client_socket;

    return response;
}

void serve_file(struct Request *request, struct Response *response, char *path) {
    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        fprintf(stderr, "Failed to read file...\n");
        return;
    }

    fseek(fp, 0, SEEK_END);
    long int file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char *body = malloc(file_size + 1);
    if (body == NULL) {
        fprintf(stderr, "Failed to allocate memory for body...\n");
        fclose(fp);
    }
    memset(body, 0, file_size + 1);
    fread(body, file_size, 1, fp);

    if (send_response(response, 200, body) < 0) {
        fprintf(stderr, "Failed to send response...\n");
    }

    free(body);
    fclose(fp);
}

int send_response(struct Response *response, int status_code, char *body) {
    if (get_reason_phrase_string(status_code) == NULL) {
        fprintf(stderr, "Unrecognized status code '%d'...\n", status_code);
        return -1;
    }

    int n, sent_bytes;
    char response_buffer[2048] = {0};
    n = sprintf(response_buffer, "%s %d %s%s\n%s",
                HTTP_VERSION,
                status_code,
                get_reason_phrase_string(status_code),
                CRLF,
                body);
    if (n <= 0) {
        fprintf(stderr, "Failed to create response...\n");
        return -1;
    }
    response_buffer[n] = 0;
    sent_bytes = send(response->client_socket, response_buffer, n, 0);
    if (sent_bytes < 0) {
        fprintf(stderr, "Failed to send response...\n");
        return -1;
    }

    return 0;
}

void free_request(struct Request *request) {
    if (request->body != NULL) {
        free(request->body), request->body = NULL;
    }
    if (request->request_line != NULL) {
        if (request->request_line->uri != NULL) {
            free(request->request_line->uri), request->request_line->uri = NULL;
        }
        if (request->request_line->http_version != NULL) {
            free(request->request_line->http_version), request->request_line->http_version = NULL;
        }
        free(request->request_line), request->request_line = NULL;
    }
    free(request), request = NULL;
}

void free_response(struct Response *response) {
    free(response), response = NULL;
}