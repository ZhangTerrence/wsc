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
    struct addrinfo hints, *res, *rp;
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
    for (rp = res; rp != NULL; rp = res->ai_next) {
        // Sets up server socket.
        server_socket = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (server_socket < 0) {
            continue;
        }

        setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(int));

        // Binds server socket to port.
        if (bind(server_socket, rp->ai_addr, rp->ai_addrlen) < 0) {
            continue;
        }

        break;
    }

    if (rp == NULL) {
        perror("Unable to connect and bind server socket...");
        exit(EXIT_FAILURE);
    }

    server.domain = rp->ai_family;
    server.type = rp->ai_socktype;
    server.protocol = rp->ai_protocol;
    server.ip_address = ip_address;
    server.port = atoi(port);
    server.max_connections = max_connections;
    server.socket = server_socket;

    server.routes = NULL;

    freeaddrinfo(res);

    // Configures server socket to listen for connections.
    if (listen(server_socket, max_connections) < 0) {
        perror("Unable to start listening for connections...");
        exit(EXIT_FAILURE);
    }

    return server;
}