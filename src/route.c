#include "route.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Route *get_route(struct Route *root, char *route) {
    if (root == NULL) {
        return NULL;
    }

    int key = strcmp(root->route, route);
    if (key == 0) {
        return root;
    } else if (key < 0) {
        return get_route(root->left, route);
    } else {
        return get_route(root->right, route);
    }
}

struct Route *add_route(struct Route *root, char *route, void (*function)(struct Request *request)) {
    if (root == NULL) {
        struct Route *new_route = malloc(sizeof(struct Route));
        new_route->route = malloc(strlen(route) + 1);
        strcpy(new_route->route, route);
        new_route->function = function;
        new_route->left = NULL;
        new_route->right = NULL;
        return new_route;
    }

    int key = strcmp(root->route, route);
    printf("%s %s\n", root->route, route);
    if (key == 0) {
        fprintf(stderr, "Duplicate route...\n");
        exit(EXIT_FAILURE);
    } else if (key < 0) {
        root->left = add_route(root->left, route, function);
    } else {
        root->right = add_route(root->right, route, function);
    }

    return root;
}

void remove_routes(struct Route *root) {
    if (root == NULL) {
        return;
    }

    remove_routes(root->left);
    remove_routes(root->right);

    free(root->route);
    root->route = NULL;
    free(root);
    root = NULL;
}