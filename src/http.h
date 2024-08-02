#ifndef HTTP_H
#define HTTP_H

#define HTTP_VERSION "HTTP/1.0"

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

int parse_request_line(struct Request *request, char *request_line);

int parse_request(struct Request *request, char *request_string);

int handle_request(int client_socket, struct Request *request);

void free_request(struct Request *request);

struct Response *create_response(int client_socket);

int send_response(struct Response *response, int status_code, char *body);

#endif
