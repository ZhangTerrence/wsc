#include "../src/server.h"
#include "../src/http.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG 1

void getPageOne(struct Request *request, struct Response *response) {
    FILE *fp = fopen("./static/pageOne.html", "r");
    if (fp == NULL) {
        return;
    }

    fseek(fp, 0, SEEK_END);
    long int file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char *body = malloc(file_size + 1);
    memset(body, 0, file_size + 1);
    fread(body, file_size, 1, fp);

    send_response(response, 200, body);

    free(body);
    fclose(fp);
}

void getPageTwo(struct Request *request, struct Response *response) {
    FILE *fp = fopen("./static/pageTwo.html", "r");
    if (fp == NULL) {
        return;
    }

    fseek(fp, 0, SEEK_END);
    long int file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char *body = malloc(file_size + 1);
    memset(body, 0, file_size + 1);
    fread(body, file_size, 1, fp);

    send_response(response, 200, body);

    free(body);
    fclose(fp);
}

int main() {
    struct Server *server = create_server("127.0.0.1", "8080", 3);
#if DEBUG
    printf("Server Socket: %d\nIP Address: %s\nPort: %d\n", server->server_socket, server->ip_address, server->port);
#endif
    server->routes = add_route(server->routes, get_method_string(GET), "/api/a", getPageOne);
    server->routes = add_route(server->routes, get_method_string(GET), "/api/b", getPageTwo);
    if (server->routes != NULL) {
        run_server(server);
    }
    free_server(server);
    exit(EXIT_SUCCESS);
}
