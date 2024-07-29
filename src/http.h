#ifndef HTTP_H
#define HTTP_H

enum RequestMethod {
    GET,
    HEAD,
    POST
};

struct Request {
    // Request Line
    enum RequestMethod method;
    char *uri;
    char *http_version;

    // Request Body
    char *body;
};

void parse_request_line(struct Request *request, char *request_line);

void parse_request(struct Request *request, char *request_string);

void handle_request(int client_socket, struct Request *request);

#endif
