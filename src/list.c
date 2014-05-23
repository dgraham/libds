#include "list.h"

void *list_next_node(struct iterator *this);

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

struct list *list_clone(struct list *this) {
    struct list *clone = list_create();
    if (!clone) {
        return NULL;
    }

    struct node *node = this->head;
    while (node) {
        if (!list_push(clone, node->value)) {
            list_destroy(clone);
            return NULL;
        }
        node = node->next;
    }

    return clone;
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

bool list_concat(struct list *this, struct list *other) {
    struct node *node = other->head;
    while (node) {
        if (!list_push(this, node->value)) {
            return false;
        }
        node = node->next;
    }

    return true;
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

bool list_unshift(struct list *this, void *item) {
    struct node *node = calloc(1, sizeof(struct node));
    if (!node) {
        return false;
    }
    node->prev = NULL;
    node->next = this->head;
    node->value = item;

    if (this->head) {
        this->head->prev = node;
    }
    this->head = node;

    if (!this->tail) {
        this->tail = node;
    }

    this->length++;
    return true;
}

void *list_shift(struct list *this) {
    struct node *node = this->head;
    if (!node) {
        return NULL;
    }

    if (this->tail == this->head) {
        this->tail = NULL;
        this->head = NULL;
    } else {
        this->head = this->head->next;
        this->head->prev = NULL;
    }

    this->length--;

    void *item = node->value;
    free(node);
    return item;
}

void *list_next_node(struct iterator *this) {
    struct node *node = this->iterable;

    if (node) {
        this->iterable = node->next;
        this->current = node->value;
        this->index++;
    } else {
        this->current = NULL;
    }

    return this->current;
}

struct iterator *list_iterator(struct list *this) {
    return iterator_create(this->head, list_next_node);
}
