#include "route.h"

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

struct Route *add_route(struct Route *root, char *route, void (*function)(void)) {
    if (root == NULL) {
        struct Route *new_route = malloc(sizeof(struct Route));
        new_route->route = route;
        new_route->function = function;
        new_route->left = NULL;
        new_route->right = NULL;
        return new_route;
    }

    int key = strcmp(root->route, route);
    if (key == 0) {
        return NULL;
    } else if (key < 0) {
        return add_route(root->left, route, function);
    } else {
        return add_route(root->right, route, function);
    }
}

struct Route *remove_route(struct Route *root, char *route) {
    if (root == NULL) {
        return NULL;
    }

    int key = strcmp(root->route, route);
    if (key == 0) {
        if (root->left == NULL) {
            struct Route *tmp = root->right;
            free(root);
            root = NULL;
            return tmp;
        } else if (root->right == NULL) {
            struct Route *tmp = root->left;
            free(root);
            root = NULL;
            return tmp;
        } else {
            struct Route *successor = root->right;
            while (successor->left != NULL) {
                successor = successor->left;
            }

            root->route = successor->route;
            root->function = successor->function;
            root->right = remove_route(root->right, successor->route);
        }
    } else if (key < 0) {
        root->left = remove_route(root->left, route);
    } else {
        root->right = remove_route(root->right, route);
    }

    return root;
}

void remove_all_routes(struct Route *root) {
    if (root == NULL) {
        return;
    }

    remove_all_routes(root->left);
    remove_all_routes(root->right);

    free(root);
    root = NULL;
}