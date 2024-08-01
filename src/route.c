#include "route.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Route *get_route(struct Route *root, char *method, char *uri) {
    if (root == NULL) {
        return NULL;
    }

    int key = strcmp(root->method, method) || strcmp(root->uri, uri);
    if (key == 0) {
        return root;
    } else if (key < 0) {
        return get_route(root->left, method, uri);
    } else {
        return get_route(root->right, method, uri);
    }
}

struct Route *add_route(struct Route *root, char *method, char *uri, void (*function)(struct Request *request)) {
    if (root == NULL) {
        struct Route *new_route = malloc(sizeof(struct Route));
        new_route->method = malloc(strlen(method) + 1);
        strcpy(new_route->method, method);
        new_route->uri = malloc(strlen(uri) + 1);
        strcpy(new_route->uri, uri);
        new_route->function = function;
        new_route->left = NULL;
        new_route->right = NULL;
        return new_route;
    }

    int key = strcmp(root->method, method) || strcmp(root->uri, uri);
    if (key == 0) {
        fprintf(stderr, "Duplicate route...\n");
        return NULL;
    } else if (key < 0) {
        root->left = add_route(root->left, method, uri, function);
    } else {
        root->right = add_route(root->right, method, uri, function);
    }

    return root;
}

void remove_routes(struct Route *root) {
    if (root == NULL) {
        return;
    }

    remove_routes(root->left);
    remove_routes(root->right);

    free(root->method);
    root->method = NULL;
    free(root->uri);
    root->uri = NULL;
    free(root);
    root = NULL;
}