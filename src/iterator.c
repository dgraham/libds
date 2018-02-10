#include "iterator.h"

/* Create an iterator to be used to loop over a data structure.
 *
 * iterable - The data structure providing the items through which to iterate.
 * next     - The function that provides the next item in the iterator loop.
 *            It must set `iter->current` to the next item and advance
 *            the loop index with `iter->index++`.
 *
 * Examples
 *
 *   struct iterator *iter = iterator_create(array, next_name);
 *   while (iter->next(iter)) {
 *       char *name = iter->current;
 *       printf("index: %lu, name: %s\n", iter->index, name);
 *   }
 *   iter->destroy(iter);
 *
 * Returns the iterator or null if memory allocation failed.
 */
struct iterator *iterator_create(void *iterable,
                                 void *(*next)(struct iterator *)) {
    struct iterator *this = calloc(1, sizeof(struct iterator));
    if (!this) {
        return NULL;
    }

    this->index = 0;
    this->iterable = iterable;
    this->current = NULL;
    this->next = next;
    this->destroy = iterator_destroy;

    return this;
}

/* Free the memory allocated for the iterator. This is typically invoked
 * through the `iter->destroy(iter)` function pointer. Iterator implementations
 * that allocate additional memory can provide their own destructor function
 * rather than use this default implementation.
 *
 * this - The iterator to deallocate.
 *
 * Returns nothing.
 */
void iterator_destroy(struct iterator *this) {
    this->index = 0;
    this->next = NULL;
    this->destroy = NULL;
    this->current = NULL;
    this->iterable = NULL;
    free(this);
}
