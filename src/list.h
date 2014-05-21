#ifndef LIST_H
#define LIST_H

#include <stdlib.h>
#include <stdbool.h>

struct list {
    void **nodes;
    size_t length;
    size_t capacity;
};

struct list *list_create();

void list_destroy(struct list *this);

#endif

