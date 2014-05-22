#include <stdlib.h>
#include "iterator.h"

struct iterator *iterator_create(void *iterable, void *(*next)(struct iterator *)) {
    struct iterator *this = calloc(1, sizeof(struct iterator));
    if (!this) {
        return NULL;
    }

    this->index = 0;
    this->iterable = iterable;
    this->current = NULL;
    this->next = next;
    this->destroy = iterator_destroy;

    return this;
}

void iterator_destroy(struct iterator *this) {
    this->index = 0;
    this->next = NULL;
    this->destroy = NULL;
    this->current = NULL;
    this->iterable = NULL;
    free(this);
}
