#ifndef ROUTE_H
#define ROUTE_H

#include "http.h"

struct Route {
    char *method;
    char *uri;

    void (*function)(struct Request *request);

    struct Route *left;
    struct Route *right;
};

struct Route *get_route(struct Route *root, char *method, char *uri);

struct Route *add_route(struct Route *root, char *method, char *uri, void (*function)(struct Request *request));

void remove_routes(struct Route *root);

#endif
