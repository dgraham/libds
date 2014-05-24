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

struct heap *heap_clone(struct heap *this);

void heap_clear(struct heap *this);

bool heap_push(struct heap *this, void *item);

void *heap_pop(struct heap *this);

bool heap_merge(struct heap *this, struct heap *other);

#endif
