#include "list.h"

bool resize(struct list *this, size_t capacity);

struct list *list_create() {
    struct list *this = calloc(1, sizeof(struct list));
    if (!this) {
        return NULL;
    }

    this->head = NULL;
    this->tail = NULL;
    this->length = 0;

    return this;
}

void list_destroy(struct list *this) {
    struct node *node = this->head;
    while (node) {
        struct node *next = node->next;
        free(node);
        node = next;
    }

    this->head = NULL;
    this->tail = NULL;
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

    if (!this->head) {
        this->head = node;
    }

    if (this->tail) {
        this->tail->next = node;
    }
    this->tail = node;

    this->length++;
    return true;
}

