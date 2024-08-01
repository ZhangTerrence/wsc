#ifndef HTTP_H
#define HTTP_H

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

char *get_method_string(enum RequestMethod method);

void parse_request_line(struct Request *request, char *request_line);

void parse_request(struct Request *request, char *request_string);

void handle_request(int client_socket, struct Request *request);

#endif
