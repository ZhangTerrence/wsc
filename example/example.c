#include "../src/server.h"
#include "../src/http.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG 1

void get_page_one(struct Request *request, struct Response *response) {
    serve_file(request, response, "./static/pageOne.html");
}

void get_page_two(struct Request *request, struct Response *response) {
    serve_file(request, response, "./static/pageTwo.html");
}

int main() {
    struct Server *server = create_server("127.0.0.1", "8080", 3);
#if DEBUG
    printf("Server Socket: %d\nIP Address: %s\nPort: %d\n", server->server_socket, server->ip_address, server->port);
#endif
    server->routes = add_route(server->routes, get_method_string(GET), "/api/a", get_page_one);
    server->routes = add_route(server->routes, get_method_string(GET), "/api/b", get_page_two);
    if (server->routes != NULL) {
        run_server(server);
    } else {
        free_server(server);
        exit(EXIT_FAILURE);
    }
}
