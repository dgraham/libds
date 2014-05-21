#include "list.h"

bool resize(struct list *this, size_t capacity);

struct list *list_create() {
    struct list *this = calloc(1, sizeof(struct list));
    if (!this) {
        return NULL;
    }

    this->nodes = NULL;
    this->length = 0;
    this->capacity = 0;

    if (!resize(this, 16)) {
        list_destroy(this);
        return NULL;
    }

    return this;
}

void list_destroy(struct list *this) {
    this->length = 0;
    this->capacity = 0;
    free(this->nodes);
    free(this);
}

bool resize(struct list *this, size_t capacity) {
    void **nodes = realloc(this->nodes, capacity * sizeof(void *));
    if (!nodes) {
        return false;
    }

    this->nodes = nodes;
    this->capacity = capacity;

    return true;
}

