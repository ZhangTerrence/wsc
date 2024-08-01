#ifndef SERVER_H
#define SERVER_H

#include "route.h"

#define REUSE_ADDR 1

struct Server {
    // Server socket information.
    int server_socket;
    int domain;
    int type;
    int protocol;

    // Server specific information.
    char *ip_address;
    int port;
    int max_connections;

    // Server routes.
    struct Route *routes;
};

struct Server create_server(char *ip_address, char *port, int max_connections);

void run_server(struct Server server);

#endif
