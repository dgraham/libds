#include <string.h>
#include "heap.h"

void heap_move_up(struct heap *this, size_t k);
void heap_move_down(struct heap *this, size_t k);
bool heap_resize(struct heap *this, size_t capacity);

/* Allocate memory for a new heap instance. Depending on the comparator
 * function behavior, it can be a min-heap or a max-heap. The heap must
 * be freed with a call to heap_destroy.
 *
 * comparator - The function with which to sort entries in the heap. It must
 *              return < 0 if a < b, 0 if a == b, and > 0 if a > b.
 *
 * Returns the new heap or null if memory allocation failed.
 */
struct heap *heap_create(int (*comparator)(const void *, const void *)) {
    struct heap *this = calloc(1, sizeof(struct heap));
    if (!this) {
        return NULL;
    }

    this->comparator = comparator;
    this->nodes = NULL;
    this->capacity = 0;
    this->size = 0;

    if (!heap_resize(this, 16)) {
        heap_destroy(this);
        return NULL;
    }

    return this;
}

/* Free the memory associated with the heap. The values stored in the heap are
 * not freed. They must be deallocated before the heap is destroyed, or
 * otherwise cleaned up later by the caller.
 *
 * this - The heap to free.
 *
 * Returns nothing.
 */
void heap_destroy(struct heap *this) {
    this->capacity = 0;
    this->size = 0;
    this->comparator = NULL;
    free(this->nodes);
    free(this);
}

/* Add a new item to the heap. The item will be sorted into position according
 * to the heap's comparator function.
 *
 * this - The heap onto which to push the item.
 * item - The data item to store
 *
 * Returns true if the item was added or false if memory allocation failed.
 */
bool heap_push(struct heap *this, void *item) {
    if (this->size == this->capacity) {
        if (!heap_resize(this, this->capacity * 2)) {
            return false;
        }
    }

    this->nodes[this->size] = item;
    this->size++;
    heap_move_up(this, this->size - 1);

    return true;
}

/* Remove the root item from the heap. The remaining items are sorted into place
 * with the heap's comparator function.
 *
 * heap - The heap from which to remove the item.
 *
 * Returns the item or null if the heap is empty.
 */
void *heap_pop(struct heap *this) {
    if (this->size == 0) {
        return NULL;
    }

    void *root = this->nodes[0];
    this->nodes[0] = this->nodes[this->size - 1];
    this->size--;
    heap_move_down(this, 0);

    return root;
}

/* Private: Move the last element in the heap up until it's in sorted order.
 *
 * this - The heap to fix up.
 * k    - The node to sort up the heap until it's in position.
 *
 * Returns nothing.
 */
void heap_move_up(struct heap *this, size_t k) {
    if (k == 0) {
        return;
    }

    size_t parent = (k - 1) / 2;
    if (this->comparator(this->nodes[k], this->nodes[parent]) < 0) {
        void *temp = this->nodes[k];
        this->nodes[k] = this->nodes[parent];
        this->nodes[parent] = temp;
        heap_move_up(this, parent);
    }
}

/* Private: Move the root element down in the heap until it's in sorted order.
 *
 * this - The heap to fix up.
 * k    - The node to move down the heap until it's in position.
 *
 * Returns nothing.
 */
void heap_move_down(struct heap *this, size_t k) {
    size_t left  = 2 * k + 1;
    size_t right = 2 * k + 2;
    if (left > (this->size - 1)) {
        return;
    }

    size_t smaller = left;
    if (right < this->size &&
            this->comparator(this->nodes[right], this->nodes[left]) < 0) {
        smaller = right;
    }

    if (this->comparator(this->nodes[k], this->nodes[smaller]) > 0) {
        void *temp = this->nodes[k];
        this->nodes[k] = this->nodes[smaller];
        this->nodes[smaller] = temp;
        heap_move_down(this, smaller);
    }
}

/* Private: Allocate memory used to store heap node pointers.
 *
 * this     - The heap to expand.
 * capacity - The new number of nodes to accomodate.
 *
 * Returns true if memory allocation succeeded.
 */
bool heap_resize(struct heap *this, size_t capacity) {
    void **nodes = realloc(this->nodes, capacity * sizeof(void *));
    if (!nodes) {
        return false;
    }

    this->nodes = nodes;
    this->capacity = capacity;
    return true;
}
