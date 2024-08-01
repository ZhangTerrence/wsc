#include "../src/server.h"

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

#define DEBUG 1

void response(struct Request *request) {
    int sent_bytes;
#if DEBUG
    printf("Method: %d\nRoute: %s\nVersion: %s\n", request->request_line->method, request->request_line->uri, request->request_line->http_version);
    printf("Body: %s\n", request->body);
#endif

    const char response[] = "HTTP/1.0 200 OK\r\n\n<html><body><h1>Hello, world.</h1><p>A simple GET request.</p></body></html>";
    sent_bytes = send(request->client_socket, response, strlen(response), 0);
    if (sent_bytes < 0) {
        perror("Unable to send response...");
    }
}

int main() {
    struct Server server = create_server("127.0.0.1", "8080", 3);
#if DEBUG
      printf("Socket: %d\nIP Address: %s\nPort: %d\n", server.server_socket, server.ip_address, server.port);
#endif
    server.routes = add_route(server.routes, "/api", response);
    run_server(server);
    remove_routes(server.routes);
}
