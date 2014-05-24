#include <string.h>
#include "heap.h"

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
