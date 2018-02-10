#include "heap.h"
#include <string.h>

static void heap_move_up(struct heap *this, size_t k);
static void heap_move_down(struct heap *this, size_t k);
static void *heap_next_node(struct iterator *this);
static void heap_destroy_iterator(struct iterator *this);
static bool heap_resize(struct heap *this, size_t capacity);

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

/* Copy the heap contents into a new heap. Modifications to either heap
 * will not affect the other. However, they both point to the same values.
 *
 * this - The heap whose entries will be cloned.
 *
 * Returns the cloned heap or null if memory allocation failed.
 */
struct heap *heap_clone(struct heap *this) {
    struct heap *clone = heap_create(this->comparator);
    if (!clone) {
        return NULL;
    }

    if (clone->capacity < this->size) {
        if (!heap_resize(clone, this->size)) {
            heap_destroy(clone);
            return NULL;
        }
    }

    memcpy(clone->nodes, this->nodes, this->size * sizeof(void *));
    clone->size = this->size;
    return clone;
}

/* Remove all entries from the heap. This does not free the values stored in
 * the heap. The caller is responsible for deallocating the value pointers.
 *
 * The heap's memory is not resized after clearing the entries. It consumes
 * the same amount of memory as it did before a call to this function. Adding
 * entries to a cleared heap avoids the cost of memory allocation.
 *
 * this - The heap to clear.
 *
 * Returns nothing.
 */
void heap_clear(struct heap *this) {
    memset(this->nodes, 0, this->capacity * sizeof(void *));
    this->size = 0;
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
    this->nodes[this->size - 1] = NULL;
    this->size--;
    if (this->size > 0) {
        heap_move_down(this, 0);
    }

    return root;
}

/* Combine one heap into another. The destination heap sorts new nodes into
 * place to maintain its total ordering. The source heap is unaffected by the
 * merge. It contains the same items after merging.
 *
 * this  - The destination heap
 * other - The source heap.
 *
 * Returns true if the heaps were merged, false if memory allocation failed.
 */
bool heap_merge(struct heap *this, struct heap *other) {
    size_t total = this->size + other->size;
    if (total > this->capacity) {
        if (!heap_resize(this, total)) {
            return false;
        }
    }

    for (size_t i = 0; i < other->size; i++) {
        if (!heap_push(this, other->nodes[i])) {
            return false;
        }
    }

    return true;
}

/* Create an external iterator with which to loop over each item in the heap
 * in sorted order. The caller must free the iterator's memory when iteration
 * is complete.
 *
 * Because heap iteration is a destructive process, items must be popped from
 * the heap to maintain total ordering, the iterator allocates a cloned heap
 * through which to iterate. The clone is deallocated through the iterator's
 * `destroy` function.
 *
 * this - The heap to iterate through.
 *
 * Examples
 *
 *   struct iterator *nodes = heap_iterator(heap);
 *   while (nodes->next(nodes)) {
 *       char *name = nodes->current;
 *       printf("%s\n", name);
 *   }
 *   nodes->destroy(nodes);
 *
 * Returns an iterator or null if memory allocation failed.
 */
struct iterator *heap_iterator(struct heap *this) {
    struct heap *clone = heap_clone(this);
    if (!clone) {
        return NULL;
    }

    struct iterator *nodes = iterator_create(clone, heap_next_node);
    if (!nodes) {
        heap_destroy(clone);
        return NULL;
    }

    nodes->destroy = heap_destroy_iterator;
    return nodes;
}

/* Private: Free the iterator's memory, including the cloned heap through
 * which it was navigating. This is the function pointer used to implement
 * `iter->destroy(iter)`.
 *
 * this - The iterator to destroy.
 *
 * Returns nothing.
 */
void heap_destroy_iterator(struct iterator *this) {
    heap_destroy(this->iterable);
    iterator_destroy(this);
}

/* Private: Move the iterator to the next node. This is the function pointer
 * used to implement `iter->next(iter)`.
 *
 * this - The iterator to advance.
 *
 * Returns the next item or null if the heap is empty.
 */
void *heap_next_node(struct iterator *this) {
    struct heap *heap = this->iterable;

    bool first = !this->current && this->index == 0;

    this->current = heap_pop(heap);

    if (!first && this->current) {
        this->index++;
    }

    return this->current;
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
    size_t left = 2 * k + 1;
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
