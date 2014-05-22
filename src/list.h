#ifndef LIST_H
#define LIST_H

#include <stdlib.h>
#include <stdbool.h>
#include "iterator.h"

struct node {
    void *value;
    struct node *prev;
    struct node *next;
};

struct list {
    struct node *head;
    struct node *tail;
    size_t length;
};

struct list *list_create();

void list_destroy(struct list *this);

struct list *list_clone(struct list *this);

bool list_push(struct list *this, void *item);

void *list_pop(struct list *this);

bool list_unshift(struct list *this, void *item);

void *list_shift(struct list *this);

void list_clear(struct list *this);

struct iterator *list_iterator(struct list *this);

#endif
