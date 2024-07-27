#ifndef ROUTE_H
#define ROUTE_H

#include "http.h"

struct Route {
    char* route;
    void (*function)(void);

    struct Route *left;
    struct Route *right;
};

struct Route *get_route(struct Route *root, char *route);

struct Route *add_route(struct Route *root, char *route, void (*function)(struct Request request));

struct Route *remove_route(struct Route *root, char *route);

void remove_all_routes(struct Route *root);

#endif
