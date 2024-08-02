#include "http.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

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

int parse_request_line(struct Request *request, char *request_line) {
    const char SP[] = " ";
    char *method = strtok(request_line, SP), *uri = strtok(NULL, SP), *http_version = strtok(NULL, SP);

    if (method == NULL) {
        fprintf(stderr, "Missing method from request...\n");
        goto return_failure;
    }
    if (uri == NULL) {
        fprintf(stderr, "Missing URI from request...\n");
        goto return_failure;
    }
    if (http_version == NULL) {
        fprintf(stderr, "Missing HTTP version from request...\n");
        goto return_failure;
    }

    if (strcmp(method, "GET") == 0) {
        request->request_line->method = GET;
    } else if (strcmp(method, "HEAD") == 0) {
        request->request_line->method = HEAD;
    } else if (strcmp(method, "POST") == 0) {
        request->request_line->method = POST;
    } else {
        fprintf(stderr, "Unrecognized method from request...\n");
        goto return_failure;
    }

    request->request_line->uri = malloc(strlen(uri) + 1);
    strcpy(request->request_line->uri, uri);
    request->request_line->http_version = malloc(strlen(http_version) + 1);
    strcpy(request->request_line->http_version, http_version);

return_success: {
        return 0;
    }
return_failure: {
        free(request);
        free(request->request_line);
        return -1;
    }
}

int parse_request(struct Request *request, char *request_string) {
    const char CRLF[] = "\r\n", *start = request_string;
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
    strcpy(request->body, body);

    return 0;
}

int handle_request(int client_socket, struct Request *request) {
    int rec_bytes;
    char buffer[4096] = {0};

    rec_bytes = recv(client_socket, buffer, 4096, 0);
    if (rec_bytes <= 0) {
        perror("Unable to read from request...");
        return -1;
    }

    return parse_request(request, buffer);
}

void free_request(struct Request *request) {
    if (request->body != NULL) {
        free(request->body);
        request->body = NULL;
    }
    if (request->request_line != NULL) {
        if (request->request_line->uri != NULL) {
            free(request->request_line->uri);
            request->request_line->uri = NULL;
        }
        if (request->request_line->http_version != NULL) {
            free(request->request_line->http_version);
            request->request_line->http_version = NULL;
        }
        free(request->request_line);
        request->request_line = NULL;
    }
    free(request);
    request = NULL;
}

struct Response *create_response(int client_socket) {
    struct Response *response = malloc(sizeof(struct Response));
    response->client_socket = client_socket;

    return response;
}

int send_response(struct Response *response, int status_code, char *body) {
    if (get_reason_phrase_string(status_code) == NULL) {
        fprintf(stderr, "Unrecognized static code...\n");
        return -1;
    }

    const char CRLF[] = "\r\n";
    int n, sent_bytes;
    char response_buffer[2048] = {0};
    n = sprintf(response_buffer, "%s %d %s%s\n%s",
                HTTP_VERSION,
                status_code,
                get_reason_phrase_string(status_code),
                CRLF,
                body);
    if (n <= 0) {
        fprintf(stderr, "Unable to create response...\n");
        return -1;
    }
    response_buffer[n] = 0;
    sent_bytes = send(response->client_socket, response_buffer, n, 0);
    if (sent_bytes < 0) {
        perror("Unable to send response...");
        return -1;
    }

    return 0;
}
