#include "route.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Route *get_route(struct Route *root, char *method, char *uri) {
    if (root == NULL) {
        return NULL;
    }

    int key_i = strcmp(root->method, method), key_j = strcmp(root->uri, uri);
    if (key_i == 0 && key_j == 0) {
        return root;
    } else if (key_i < 0 || key_j < 0) {
        return get_route(root->left, method, uri);
    } else {
        return get_route(root->right, method, uri);
    }
}

struct Route *add_route(struct Route *root, char *method, char *uri, void (*function)(struct Request *request, struct Response *response)) {
    if (method == NULL || uri == NULL || function == NULL) {
        return NULL;
    }
    if (root == NULL) {
        struct Route *new_route = malloc(sizeof(struct Route));
        if (new_route == NULL) {
            fprintf(stderr, "Failed to allocate memory for new route...\n");
            return NULL;
        }
        new_route->method = malloc(strlen(method) + 1);
        if (new_route->method == NULL) {
            fprintf(stderr, "Failed to allocate memory for new route's method...\n");
            free(new_route), new_route = NULL;
            return NULL;
        }
        strcpy(new_route->method, method);
        new_route->uri = malloc(strlen(uri) + 1);
        if (new_route->method == NULL) {
            fprintf(stderr, "Failed to allocate memory for new route's URI...\n");
            free(new_route->method), new_route->method = NULL;
            free(new_route), new_route = NULL;
            return NULL;
        }
        strcpy(new_route->uri, uri);
        new_route->function = function;
        new_route->left = NULL;
        new_route->right = NULL;
        return new_route;
    }

    int key_i = strcmp(root->method, method), key_j = strcmp(root->uri, uri);
    printf("%s %s %s %s %d %d\n", method, uri, root->method, root->uri, key_i, key_j);
    if (key_i == 0 && key_j == 0) {
        fprintf(stderr, "Duplicate route '%s' '%s'...\n", method, uri);
        free_routes(root);
        return NULL;
    } else if (key_i < 0 || key_j < 0) {
        root->left = add_route(root->left, method, uri, function);
        if (root->left == NULL) {
            free_routes(root);
            return NULL;
        }
    } else {
        root->right = add_route(root->right, method, uri, function);
        if (root->right == NULL) {
            free_routes(root);
            return NULL;
        }
    }

    return root;
}

void free_routes(struct Route *root) {
    if (root == NULL) {
        return;
    }

    free_routes(root->left);
    free_routes(root->right);

    if (root->method != NULL) {
        free(root->method), root->method = NULL;
    }
    if (root->uri != NULL) {
        free(root->uri), root->uri = NULL;
    }
    free(root), root = NULL;
}
