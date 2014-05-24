#ifndef HEAP_H
#define HEAP_H

#include <stdlib.h>
#include <stdbool.h>
#include "iterator.h"

struct heap {
    int (*comparator)(const void *, const void *);
    void **nodes;
    size_t capacity;
    size_t size;
};

struct heap *heap_create(int (*comparator)(const void *, const void *));

void heap_destroy(struct heap *this);

bool heap_push(struct heap *this, void *item);

#endif
