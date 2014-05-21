#include "list.h"

bool resize(struct list *this, size_t capacity);

struct list *list_create() {
    struct list *this = calloc(1, sizeof(struct list));
    if (!this) {
        return NULL;
    }

    this->first = NULL;
    this->last = NULL;
    this->length = 0;

    return this;
}

void list_destroy(struct list *this) {
    struct node *node = this->first;
    while (node) {
        struct node *next = node->next;
        free(node);
        node = next;
    }

    this->first = NULL;
    this->last = NULL;
    this->length = 0;

    free(this);
}

bool list_push(struct list *this, void *item) {
    struct node *node = calloc(1, sizeof(struct node));
    if (!node) {
        return false;
    }
    node->next = NULL;
    node->value = item;

    if (!this->first) {
        this->first = node;
    }

    if (this->last) {
        this->last->next = node;
    }
    this->last = node;

    this->length++;
    return true;
}

