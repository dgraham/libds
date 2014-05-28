#include "list.h"

void *list_next_node(struct iterator *this);

/* Allocate memory for a new linked list instance.
 *
 * Returns the new list or null if memory allocation failed.
 */
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

/* Free the memory used by the list. This does not free any values stored in
 * the list. They must be freed by the caller.
 *
 * this - The list to deallocate.
 *
 * Returns nothing.
 */
void list_destroy(struct list *this) {
    list_clear(this);
    free(this);
}

/* Copy the list contents into a new list. Future modifications to either list
 * will not affect the other, although they both point to the same values.
 *
 * this - The list to copy.
 *
 * Returns a new list or null if memory allocation failed.
 */
struct list *list_clone(struct list *this) {
    struct list *clone = list_create();
    if (!clone) {
        return NULL;
    }

    struct lnode *node = this->head;
    while (node) {
        if (!list_push(clone, node->value)) {
            list_destroy(clone);
            return NULL;
        }
        node = node->next;
    }

    return clone;
}

/* Remove all nodes from the list. This does not free the values stored in
 * the list. The caller is responsible for deallocating the value pointers.
 *
 * this - The list to clear.
 *
 * Returns nothing.
 */
void list_clear(struct list *this) {
    struct lnode *node = this->head;
    while (node) {
        struct lnode *next = node->next;
        free(node);
        node = next;
    }

    this->head = NULL;
    this->tail = NULL;
    this->length = 0;
}

/* Concatenate one list into another. The source list is unchanged.
 *
 * this  - The destination list.
 * other - The source list.
 *
 * Returns false if memory allocation failed.
 */
bool list_concat(struct list *this, struct list *other) {
    struct lnode *node = other->head;
    while (node) {
        if (!list_push(this, node->value)) {
            return false;
        }
        node = node->next;
    }

    return true;
}

/* Add an item to the end of the list.
 *
 * this - The list to receive the item.
 * item - The data to append to the list.
 *
 * Returns false if memory allocation failed.
 */
bool list_push(struct list *this, void *item) {
    struct lnode *node = calloc(1, sizeof(struct lnode));
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

/* Remove the last item in the list. Used together with `push`, a linked list
 * can be used as a stack.
 *
 * this - The list to pop.
 *
 * Returns the last item or null if the list is empty.
 */
void *list_pop(struct list *this) {
    struct lnode *node = this->tail;
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

/* Add an item to the front of the list.
 *
 * this - The list that receives the item.
 * item - The data to store in the list.
 *
 * Returns false if memory allocation failed.
 */
bool list_unshift(struct list *this, void *item) {
    struct lnode *node = calloc(1, sizeof(struct lnode));
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

/* Remove the first item from the list. Used together with `push`, a linked
 * list can be used as a queue. This performs better than vector because a
 * memory copy is not required to move all items up one position after the
 * shift.
 *
 * this - The list to shift.
 *
 * Returns the item or null if the list is empty.
 */
void *list_shift(struct list *this) {
    struct lnode *node = this->head;
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

/* Private: Move the iterator to the next list node. This is the function
 * pointer used to implement `iter->next(iter)`.
 *
 * this - The iterator to advance.
 *
 * Returns the next node in the iterator.
 */
void *list_next_node(struct iterator *this) {
    struct lnode *node = this->iterable;

    bool first = !this->current && this->index == 0;

    if (node) {
        this->iterable = node->next;
        this->current = node->value;
        if (!first) {
            this->index++;
        }
    } else {
        this->current = NULL;
    }

    return this->current;
}

/* Create an external iterator with which to loop over each item in the list.
 * The caller must free the iterator's memory when iteration is complete.
 *
 * this - The list to iterate through.
 *
 * Examples
 *
 *   struct iterator *nodes = list_iterator(list);
 *   while (nodes->next(nodes)) {
 *       char *name = nodes->current;
 *       printf("%s\n", name);
 *   }
 *   nodes->destroy(nodes);
 *
 * Returns an iterator or null if memory allocation failed.
 */
struct iterator *list_iterator(struct list *this) {
    return iterator_create(this->head, list_next_node);
}
