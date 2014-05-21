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
    list_clear(this);
    free(this);
}

void list_clear(struct list *this) {
    struct node *node = this->head;
    while (node) {
        struct node *next = node->next;
        free(node);
        node = next;
    }

    this->head = NULL;
    this->tail = NULL;
    this->length = 0;
}

bool list_push(struct list *this, void *item) {
    struct node *node = calloc(1, sizeof(struct node));
    if (!node) {
        return false;
    }
    node->prev = NULL;
    node->next = NULL;
    node->value = item;

    if (!this->head) {
        this->head = node;
    }

    if (this->tail) {
        this->tail->next = node;
    }
    node->prev = this->tail;
    this->tail = node;

    this->length++;
    return true;
}

void *list_pop(struct list *this) {
    struct node *node = this->tail;
    if (!node) {
        return NULL;
    }

    if (this->tail == this->head) {
        this->tail = NULL;
        this->head = NULL;
    } else {
        this->tail = this->tail->prev;
        this->tail->next = NULL;
    }

    this->length--;

    void *item = node->value;
    free(node);
    return item;
}

