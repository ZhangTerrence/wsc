#include "server.h"
#include "route.h"
#include "http.h"

#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define DEBUG 1

struct Server *create_server(char *ip_address, char *port, int max_connections) {
    int server_socket, reuse_addr = REUSE_ADDR;
    struct addrinfo hints, *res, *p;
    struct Server *server = malloc(sizeof(struct Server));

    if (server == NULL) {
        fprintf(stderr, "Failed to allocate memory for server...\n");
        free(server);
        server = NULL;
        exit(EXIT_FAILURE);
    }

    memset(&hints, 0, sizeof(hints));
    hints.ai_addr = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    // Gets address info.
    if (getaddrinfo(ip_address, port, &hints, &res) != 0) {
        fprintf(stderr, "Failed to get address information..\n.");
        free(server);
        server = NULL;
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
        fprintf(stderr, "Failed to connect and bind server socket...\n");
        freeaddrinfo(res), free(server);
        res = NULL, server = NULL;
        exit(EXIT_FAILURE);
    }

    server->server_socket = server_socket;
    server->domain = p->ai_family;
    server->type = p->ai_socktype;
    server->protocol = p->ai_protocol;
    server->ip_address = ip_address;
    server->port = atoi(port);
    server->max_connections = max_connections;
    server->routes = NULL;

    freeaddrinfo(res);

    // Configures server socket to listen for connections.
    if (listen(server_socket, max_connections) < 0) {
        fprintf(stderr, "Failed to prepare listening for connections...\n");
        free(server);
        server = NULL;
        exit(EXIT_FAILURE);
    }

    return server;
}

void run_server(struct Server *server) {
    socklen_t address_size;
    struct sockaddr_storage client_address;

    for (;;) {
        int pid, client_socket;
        address_size = sizeof(&client_address);

        if ((client_socket = accept(server->server_socket, (struct sockaddr *) &client_address, &address_size)) < 0) {
            fprintf(stderr, "Failed to accept incoming connection...\n");
            continue;
        }

        // One child process per client connection.
        if ((pid = fork()) < 0) {
            fprintf(stderr, "Failed to create child process for connection...\n");
            close(client_socket);
            continue;
        }

        if (pid != 0) {
            // Parent
            close(client_socket);
            continue;
        }

        // Child
        close(server->server_socket);

        // Creates a new request struct and adds info to it.
        struct Request *request = malloc(sizeof(struct Request));
        if (request == NULL) {
            fprintf(stderr, "Failed to allocate memory for request...\n");
            free_server(server);
            close(client_socket);
            exit(EXIT_FAILURE);
        }
        request->request_line = malloc(sizeof(struct RequestLine));
        if (request->request_line == NULL) {
            fprintf(stderr, "Failed to allocate memory for request line...\n");
            free_server(server);
            close(client_socket);
            exit(EXIT_FAILURE);
        }
        request->client_socket = client_socket;
        if (handle_request(client_socket, request) < 0) {
            free_server(server);
            close(client_socket);
            exit(EXIT_FAILURE);
        }

        // Creates a new response struct.
        struct Response *response = create_response(client_socket);

        // Runs the specific function associated with the given route if it exists.
        char *method = get_method_string(request->request_line->method), *uri = request->request_line->uri;
        if (method == NULL) {
            fprintf(stderr, "Unrecognized method '%s'...\n", method);
            free_request(request), free_response(response);
            free_server(server);
            close(client_socket);
            exit(EXIT_FAILURE);
        }
        struct Route *route = get_route(server->routes, method, uri);
        if (route == NULL) {
            fprintf(stderr, "Unrecognized route '%s' '%s'...\n", method, uri);
            free_request(request), free_response(response);
            free_server(server);
            close(client_socket);
            exit(EXIT_FAILURE);
        }
        route->function(request, response);

#ifdef DEBUG
        printf(
            "====================================\nClient Socket: %d\nRequest Method: %s\nRequest URI: %s\n====================================\n",
            client_socket,
            get_method_string(request->request_line->method),
            request->request_line->uri);
#endif

        free_request(request), free_response(response);
        free_server(server);
        close(client_socket);
        exit(EXIT_SUCCESS);
    }
}

void free_server(struct Server *server) {
    if (server->routes != NULL) {
        free_routes(server->routes);
    }
    free(server), server = NULL;
}
