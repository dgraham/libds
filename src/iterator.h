#ifndef ITERATOR_H
#define ITERATOR_H

#include <stdlib.h>

struct iterator {
    size_t index;
    void *iterable;
    void *current;
    void *(*next)(struct iterator *this);
    void (*destroy)(struct iterator *this);
};

struct iterator *iterator_create(void *iterable,
                                 void *(*next)(struct iterator *));

void iterator_destroy(struct iterator *this);

#endif
