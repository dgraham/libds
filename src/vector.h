#ifndef VECTOR_H
#define VECTOR_H

#include <stdlib.h>
#include <stdbool.h>
#include "iterator.h"

struct vector {
    void **items;
    size_t length;
    size_t capacity;
};

struct vector *vector_create(void);

void vector_destroy(struct vector *this);

struct vector *vector_clone(struct vector *this);

bool vector_concat(struct vector *this, struct vector *other);

void vector_clear(struct vector *this);

void *vector_shift(struct vector *this);

bool vector_unshift(struct vector *this, void *item);

bool vector_push(struct vector *this, void *item);

void *vector_pop(struct vector *this);

#endif
