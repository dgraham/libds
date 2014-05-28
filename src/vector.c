#include <string.h>
#include "vector.h"

bool vector_resize(struct vector *this, size_t capacity);

/* Allocate memory for a new vector. The memory must be freed with a
 * subsequent call to `vector_destroy`.
 *
 * Returns the vector or null if memory allocation failed.
 */
struct vector *vector_create() {
    struct vector *this = calloc(1, sizeof(struct vector));
    if (!this) {
        return NULL;
    }

    this->items = NULL;
    this->length = 0;
    this->capacity = 0;

    if (vector_resize(this, 16) < 0) {
        vector_destroy(this);
        return NULL;
    }

    return this;
}

/* Deallocate the memory associated with this vector. This does not free the
 * memory for the items in the vector. The caller must free those separately.
 *
 * this - The vector to free.
 *
 * Returns nothing.
 */
void vector_destroy(struct vector *this) {
    this->length = 0;
    this->capacity = 0;
    free(this->items);
    free(this);
}

/* Private: Allocate memory to store list item pointers.
 *
 * this     - The list to resize.
 * capacity - The number of items to accomodate in the list.
 *
 * Returns false if memory allocation failed.
 */
bool vector_resize(struct vector *this, size_t capacity) {
    void **items = realloc(this->items, capacity * sizeof(void *));
    if (!items) {
        return false;
    }

    this->items = items;
    this->capacity = capacity;

    return true;
}
