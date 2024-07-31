#include "http.h"
#include "route.h"
#include "server.h"

#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct Server create_server(char *ip_address, char *port, int max_connections) {
    int server_socket, reuse_addr = REUSE_ADDR;
    struct addrinfo hints, *res, *p;
    struct Server server;

    memset(&hints, 0, sizeof(hints));
    hints.ai_addr = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    // Gets address info.
    if (getaddrinfo(ip_address, port, &hints, &res) != 0) {
        perror("Unable to get address info...");
        exit(EXIT_FAILURE);
    }

    server_socket = -1;
    for (p = res; p != NULL; p = res->ai_next) {
        // Sets up server socket.
        server_socket = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (server_socket < 0) {
            continue;
        }

        setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(int));

        // Binds server socket to port.
        if (bind(server_socket, p->ai_addr, p->ai_addrlen) < 0) {
            continue;
        }

        break;
    }

    if (p == NULL) {
        perror("Unable to connect and bind server socket...");
        exit(EXIT_FAILURE);
    }

    server.socket = server_socket;
    server.domain = p->ai_family;
    server.type = p->ai_socktype;
    server.protocol = p->ai_protocol;
    server.ip_address = ip_address;
    server.port = atoi(port);
    server.max_connections = max_connections;
    server.routes = NULL;

    freeaddrinfo(res);

    // Configures server socket to listen for connections.
    if (listen(server_socket, max_connections) < 0) {
        perror("Unable to start listening for connections...");
        exit(EXIT_FAILURE);
    }

    return server;
}

void run_server(struct Server server) {
    socklen_t address_size;
    struct sockaddr_storage client_address;

    for (;;) {
        int pid, client_socket;
        address_size = sizeof(&client_address);

        if ((client_socket = accept(server.socket, (struct sockaddr*)&client_address, &address_size)) < 0) {
            perror("Unable to accept incoming connection...");
            exit(EXIT_FAILURE);
        }

        // One child process per client connection.
        if ((pid = fork()) < 0) {
            perror("Unable to create child process...");
            exit(EXIT_FAILURE);
        }

        if (pid != 0) {
            // Parent
            close(client_socket);
            continue;
        }

        // Child
        close(server.socket);

        // Creates a new request struct and adds info to it.
        struct Request *request = malloc(sizeof(struct Request));
        handle_request(client_socket, request);

        // Runs the specific function associated with the given route if it exists.
        struct Route *route = get_route(server.routes, request->uri);
        if (route == NULL) {
            fprintf(stderr, "Unable to find route...\n");
            remove_routes(server.routes);
            free(request->uri);
            free(request->http_version);
            free(request->body);
            free(request);
            exit(EXIT_FAILURE);
        }
        route->function(request);

        remove_routes(server.routes);
        free(request->uri);
        free(request->http_version);
        free(request->body);
        free(request);
        exit(EXIT_SUCCESS);
    }
}