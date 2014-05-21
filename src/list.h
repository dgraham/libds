#ifndef LIST_H
#define LIST_H

#include <stdlib.h>
#include <stdbool.h>

struct node {
    void *value;
    struct node *next;
};

struct list {
    struct node *head;
    struct node *tail;
    size_t length;
};

struct list *list_create();

void list_destroy(struct list *this);

bool list_push(struct list *this, void *item);

#endif

