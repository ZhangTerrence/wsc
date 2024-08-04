#ifndef HTTP_H
#define HTTP_H

#define HTTP_VERSION "HTTP/1.0"

#define INTERNAL_SERVER_ERROR "HTTP/1.0 500 Internal Server Error"

enum RequestMethod {
    GET,
    HEAD,
    POST
};

struct RequestLine {
    enum RequestMethod method;
    char *uri;
    char *http_version;
};

struct RequestHeader {
};

struct Request {
    int client_socket;

    struct RequestLine *request_line;
    struct RequestHeader *request_header;

    char *body;
};

struct Response {
    int client_socket;
};

char *get_method_string(enum RequestMethod method);

int handle_request(int client_socket, struct Request *request);

struct Response *create_response(int client_socket);

int send_response(struct Response *response, int status_code, char *body);

void free_request(struct Request *request);

void free_response(struct Response *response);

#endif
