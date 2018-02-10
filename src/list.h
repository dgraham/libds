#ifndef LIST_H
#define LIST_H

#include "iterator.h"
#include <stdbool.h>
#include <stdlib.h>

struct lnode {
    void *value;
    struct lnode *prev;
    struct lnode *next;
};

struct list {
    struct lnode *head;
    struct lnode *tail;
    size_t length;
};

struct list *list_create(void);

void list_destroy(struct list *this);

struct list *list_clone(struct list *this);

bool list_concat(struct list *this, struct list *other);

bool list_push(struct list *this, void *item);

void *list_pop(struct list *this);

bool list_unshift(struct list *this, void *item);

void *list_shift(struct list *this);

void list_clear(struct list *this);

struct iterator *list_iterator(struct list *this);

#endif
