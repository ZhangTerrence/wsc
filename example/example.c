#include "../src/server.h"
#include "../src/http.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG 1

void get_page_one(struct Request *request, struct Response *response) {
    FILE *fp = fopen("./static/pageOne.html", "r");
    if (fp == NULL) {
        fprintf(stderr, "Unable to read file...\n");
        return;
    }

    fseek(fp, 0, SEEK_END);
    long int file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char *body = malloc(file_size + 1);
    if (body == NULL) {
        fprintf(stderr, "Unable to allocate memory for body...\n");
        fclose(fp);
    }
    memset(body, 0, file_size + 1);
    fread(body, file_size, 1, fp);

    if (send_response(response, 200, body) < 0) {
        fprintf(stderr, "An error has occurred...\n");
    }

    free(body);
    fclose(fp);
}

void get_page_two(struct Request *request, struct Response *response) {
    FILE *fp = fopen("./static/pageTwo.html", "r");
    if (fp == NULL) {
        fprintf(stderr, "Unable to read file...\n");
        return;
    }

    fseek(fp, 0, SEEK_END);
    long int file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char *body = malloc(file_size + 1);
    if (body == NULL) {
        fprintf(stderr, "Unable to allocate memory for body...\n");
        fclose(fp);
    }
    memset(body, 0, file_size + 1);
    fread(body, file_size, 1, fp);

    if (send_response(response, 200, body) < 0) {
        fprintf(stderr, "An error has occurred...\n");
    }

    free(body);
    fclose(fp);
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
