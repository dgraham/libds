#include <string.h>
#include "vector.h"

bool vector_resize(struct vector *this, size_t capacity);
void *vector_next_item(struct iterator *this);

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
    vector_clear(this);
    this->capacity = 0;
    free(this->items);
    free(this);
}

/* Copy the vector's contents into a new vector instance. The clone instance
 * must be freed with `vector_destroy`.
 *
 * this - The vector to copy.
 *
 * Returns a new vector or null if memory allocation failed.
 */
struct vector *vector_clone(struct vector *this) {
    struct vector *clone = vector_create();
    if (!clone) {
        return NULL;
    }

    if (clone->capacity < this->length) {
        if (!vector_resize(clone, this->length)) {
            vector_destroy(clone);
            return NULL;
        }
    }

    memcpy(clone->items, this->items, this->length * sizeof(void *));
    clone->length = this->length;

    return clone;
}

/* Remove all items from a vector. The items themselves are not freed. The
 * caller must free the data items. The vector does not resize after removing
 * the items. The memory is still allocated for future inserts.
 *
 * this - The vector to clear.
 *
 * Returns nothing.
 */
void vector_clear(struct vector *this) {
    memset(this->items, 0, this->capacity * sizeof(void *));
    this->length = 0;
}

/* Retrieve the item stored at an index. The index is bounds-checked. If
 * the index points past the end of the vector, null is returned.
 *
 * this  - The vector from which to retrieve the item.
 * index - The zero-based item index.
 *
 * Returns the item or null if the index is out of bounds.
 */
void *vector_get(struct vector *this, size_t index) {
    if (this->length == 0 || index > this->length - 1) {
        return NULL;
    }
    return this->items[index];
}

/* Store an item at an index. Any previous item is not freed when it's
 * replaced. The previous item is returned for the caller to free as needed.
 *
 * If the index is past the end of the vector, it is not inserted and null
 * is returned. Use `push` or `unshift` to expand the vector with new items.
 *
 * this  - The vector to hold the item.
 * index - The index at which to store the new item.
 * item  - The data to store in the vector.
 *
 * Returns the previous item stored at the index or null.
 */
void *vector_set(struct vector *this, size_t index, void *item) {
    if (this->length == 0 || index > this->length - 1) {
        return NULL;
    }

    void *evicted = this->items[index];
    this->items[index] = item;
    return evicted;
}

/* Remove the item at the index and shrink the vector by one. The caller must
 * free the item.
 *
 * this  - The vector to shrink.
 * index - The item index to remove from the vector.
 *
 * Returns the item previously stored at the index or null.
 */
void *vector_remove(struct vector *this, size_t index) {
    if (this->length == 0 || index > this->length - 1) {
        return NULL;
    }

    void *evicted = this->items[index];

    if (index < this->length - 1) {
        size_t length = (this->length - index - 1) * sizeof(void *);
        memmove(this->items + index, this->items + index + 1, length);
    }

    this->items[this->length - 1] = NULL;
    this->length--;

    return evicted;
}

/* Create a new vector from a range of an existing vector. The returned vector
 * must be deallocated with `vector_destroy`. The source vector is unchanged.
 *
 * this   - The vector from which to slice elements.
 * start  - The index to begin the range.
 * length - The number of elements to return.
 *
 * Returns the vector subset or null if memory allocation failed.
 */
struct vector *vector_slice(struct vector *this, size_t start, size_t length) {
    struct vector *slice = vector_create();
    if (!slice) {
        return NULL;
    }

    if (this->length == 0 || length == 0 || start > this->length - 1) {
        return slice;
    }

    size_t end = start + length - 1;
    if (end > this->length - 1) {
        end = this->length - 1;
    }

    size_t total = end - start + 1;

    if (slice->capacity < total) {
        if (!vector_resize(slice, total)) {
            vector_destroy(slice);
            return NULL;
        }
    }

    memcpy(slice->items, this->items + start, total * sizeof(void *));

    slice->length = total;
    return slice;
}

/* Add an item to the end of the vector. Expands the vector's capacity to
 * store the additional data.
 *
 * this - The vector to store the new item.
 * item - The data to append to the list.
 *
 * Returns false if memory allocation failed.
 */
bool vector_push(struct vector *this, void *item) {
    if (this->length == this->capacity) {
        if (!vector_resize(this, this->capacity * 2)) {
            return false;
        }
    }

    this->items[this->length] = item;
    this->length++;

    return true;
}

/* Remove the last item from the list. When used with `push`, the vector can
 * be used as a stack.
 *
 * this - The vector to pop.
 *
 * Returns the last item or null if the list is empty.
 */
void *vector_pop(struct vector *this) {
    if (this->length == 0) {
        return NULL;
    }

    void *item = this->items[this->length - 1];
    this->items[this->length - 1] = NULL;
    this->length--;
    return item;
}

/* Remove the first item from the list. This is a relatively expensive
 * operation because all following item pointers must be shifted one position
 * to the front of the list. A linked list is more efficient for use as a
 * queue.
 *
 * this - The vector to shift off an item.
 *
 * Returns the first item or null if the vector is empty.
 */
void *vector_shift(struct vector *this) {
    if (this->length == 0) {
        return NULL;
    }

    this->length--;
    void *item = this->items[0];
    memmove(this->items, this->items + 1, this->length * sizeof(void *));
    return item;
}

/* Add an item to the front of the list. This is relatively expensive because
 * all other item pointers must shift down one position to accomodate the new
 * first item.
 *
 * this - The vector to store the item.
 * item - The data to add to the list.
 *
 * Returns false if memory allocation failed.
 */
bool vector_unshift(struct vector *this, void *item) {
    if (this->length == this->capacity) {
        if (!vector_resize(this, this->capacity * 2)) {
            return false;
        }
    }

    memmove(this->items + 1, this->items, this->length * sizeof(void *));
    this->items[0] = item;
    this->length++;

    return true;
}

/* Concatenate a vector's items onto the end of another vector. The source
 * vector is unchanged.
 *
 * this  - The destination vector.
 * other - The source vector.
 *
 * Returns false if memory allocation fails.
 */
bool vector_concat(struct vector *this, struct vector *other) {
    size_t total = this->length + other->length;
    if (this->capacity < total) {
        if (!vector_resize(this, total)) {
            return false;
        }
    }

    memcpy(this->items + this->length, other->items, other->length * sizeof(void *));
    this->length = total;

    return true;
}

/* Sort a vector in-place.
 *
 * The array being sorted is an array of pointers to the items in the vector.
 * The comparator function receives two pointers to item memory that must be
 * cast and dereferenced to compare the actual data stored in those slots. See
 * the example below for a comparison of a vector of string items.
 *
 * this       - The vector to sort.
 * comparator - The function used to compare two items. Returns -1, 0, or 1 if
 *              the item is less than, equal to, or greater than the other item.
 *
 * Examples
 *
 *   int compare(const void *a, const void *b) {
 *       const char **a2 = (const char **) a;
 *       const char **b2 = (const char **) b;
 *       return strcmp(*a2, *b2);
 *   }
 *
 *   vector_sort(vector, compare);
 *
 * Returns nothing.
 */
void vector_sort(struct vector *this, int (*comparator)(const void *, const void *)) {
    qsort(this->items, this->length, sizeof(void *), comparator);
}

/* Create an external iterator with which to loop over each item in the list.
 * The caller must free the iterator's memory when iteration is complete.
 *
 * this - The vector to iterate through.
 *
 * Examples
 *
 *   struct iterator *items = vector_iterator(vector);
 *   while (items->next(items)) {
 *       char *name = items->current;
 *       printf("index: %lu, name: %s\n", items->index, name);
 *   }
 *   items->destroy(items);
 *
 * Returns an iterator or null if memory allocation failed.
 */
struct iterator *vector_iterator(struct vector *this) {
    return iterator_create(this, vector_next_item);
}

/* Private: Advance the iterator to the next item in the list.
 *
 * this - The iterator to advance.
 *
 * Returns the next item or null if iteration is complete.
 */
void *vector_next_item(struct iterator *this) {
    struct vector *list = this->iterable;

    if (this->index == list->length) {
        this->current = NULL;
    } else {
        this->current = list->items[this->index];
        this->index++;
    }

    return this->current;
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
