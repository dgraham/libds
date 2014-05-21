#include "list.h"

struct list *list_create() {
    struct list *this = calloc(1, sizeof(struct list));
    if (!list) {
        return NULL:
    }

    this->nodes = NULL;

    return this;
}

void list_destroy(struct list *this) {
    free(this->nodes);
    free(this);
}

