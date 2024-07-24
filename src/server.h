#ifndef SERVER_H
#define SERVER_H

#define REUSE_ADDR 1

struct Server {
    int domain;
    int type;
    int protocol;
    char *ip_address;
    int port;
    int max_connections;
    int socket;
};

struct Server create_server(char *ip_address, char *port, int max_connections);

#endif
