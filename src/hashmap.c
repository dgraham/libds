#include "hashmap.h"
#include <errno.h>
#include <stdint.h>
#include <string.h>

#define MAX_LOAD_FACTOR .75

static bool hashmap_resize(struct hashmap *this, size_t capacity);
static void *hashmap_next_entry(struct iterator *this);

static struct hentry *hentry_create(struct hkey *key, void *value);
static void hentry_destroy(struct hentry *entry);

static struct hkey *hkey_create(const void *data, size_t length);
static void hkey_destroy(struct hkey *key);
static bool hkey_equals(struct hkey *a, struct hkey *b);
static uint32_t hkey_hash(const void *data, size_t length);

/* Allocate and initialize memory for a new hashmap. The map must be freed
 * later with a call to `hashmap_destroy`.
 *
 * Returns the map or null if allocation failed.
 */
struct hashmap *hashmap_create() {
    struct hashmap *this = calloc(1, sizeof(struct hashmap));
    if (!this) {
        return NULL;
    }

    this->entries = NULL;
    this->head = NULL;
    this->tail = NULL;
    this->capacity = 0;
    this->size = 0;

    if (!hashmap_resize(this, 16)) {
        hashmap_destroy(this);
        return NULL;
    }

    return this;
}

/* Free the memory associated with this hashmap. The values stored in the
 * map are not freed. They must be freed by the caller. See the examples
 * section for a method of cleaning all memory associated with the map.
 *
 * this - The hashmap to free.
 *
 * Examples
 *
 *   struct iterator *entries = hashmap_iterator(map);
 *   while (entries->next(entries)) {
 *       struct hentry *entry = entries->current;
 *       free(entry->value);
 *   }
 *   entries->destroy(entries);
 *   hashmap_destroy(map);
 *
 * Returns nothing.
 */
void hashmap_destroy(struct hashmap *this) {
    hashmap_clear(this);
    free(this->entries);
    this->entries = NULL;
    this->capacity = 0;
    free(this);
}

/* Copy all key/value pairs into a new hashmap instance.
 *
 * this - The hashmap to clone.
 *
 * Returns the cloned hashmap or null if memory allocation failed.
 */
struct hashmap *hashmap_clone(struct hashmap *this) {
    struct hashmap *clone = hashmap_create();
    if (!clone) {
        return NULL;
    }

    if (!hashmap_merge(clone, this)) {
        hashmap_destroy(clone);
        return NULL;
    }

    return clone;
}

/* Remove all key/value entries from the hashmap. This does not free the
 * memory associated with the values referenced in the map. The caller
 * must free the values before clearing the map.
 *
 * this - The map to clear.
 *
 * Returns nothing.
 */
void hashmap_clear(struct hashmap *this) {
    struct hentry *entry = this->head;
    while (entry) {
        struct hentry *next = entry->next;
        hentry_destroy(entry);
        entry = next;
    }

    memset(this->entries, 0, this->capacity * sizeof(struct hentry *));

    this->head = NULL;
    this->tail = NULL;
    this->size = 0;
}

/* Store a key and value pair in the hash table. If a previous value was stored
 * at the key, it's returned so that it may be freed by the caller.
 *
 * this  - The hashmap in which to store the value.
 * key   - The unique key to which to map the value. The caller must free this
 *         key if it was heap allocated. The map maintains its own internal key
 *         memory for the entry.
 * value - The value to be looked up by its key.
 *
 * Examples
 *
 *    int id = 42;
 *    char *value = "foo";
 *    struct hkey key = {&id, sizeof(id)};
 *    hashmap_set(map, &key, value);
 *
 * Returns the previous value or null. The `errno` global is set to non-zero if
 * the set failed, zero if the value was stored successfully.
 */
void *hashmap_set(struct hashmap *this, struct hkey *key, void *value) {
    uint32_t hashed = hkey_hash(key->data, key->length);
    size_t bucket = hashed % this->capacity;
    struct hentry *entry = this->entries[bucket];

    errno = 0;
    while (entry) {
        if (hkey_equals(entry->key, key)) {
            void *evicted = entry->value;
            entry->value = value;
            return evicted;
        }
        entry = entry->chain;
    }

    entry = hentry_create(key, value);
    if (!entry) {
        return NULL;
    }

    entry->chain = this->entries[bucket];
    this->entries[bucket] = entry;

    if (!this->head) {
        this->head = entry;
    }

    if (this->tail) {
        this->tail->next = entry;
        entry->prev = this->tail;
    }
    this->tail = entry;

    this->size++;

    double load = (double)this->size / this->capacity;
    if (load > MAX_LOAD_FACTOR) {
        if (!hashmap_resize(this, this->capacity * 2)) {
            return NULL;
        }
    }

    return NULL;
}

/* Retrieve the value stored at the key.
 *
 * this - The hashmap from which to retrieve the value.
 * key  - The key to look up in the map.
 *
 * Examples
 *
 *    int id = 42;
 *    struct hkey key = {&id, sizeof(id)};
 *    char *found = hashmap_get(map, &key);
 *    if (found) {
 *        printf("found id %d value: %s\n", id, found);
 *    }
 *
 * Returns the value or null if not found.
 */
void *hashmap_get(struct hashmap *this, struct hkey *key) {
    uint32_t hashed = hkey_hash(key->data, key->length);
    size_t bucket = hashed % this->capacity;
    struct hentry *entry = this->entries[bucket];

    while (entry) {
        if (hkey_equals(entry->key, key)) {
            return entry->value;
        }
        entry = entry->chain;
    }

    return NULL;
}

/* Determine if the key is contained within the hashmap. Useful for cases
 * where the map is being used as a set, storing keys with null values.
 *
 * this - The hashmap to query.
 * key  - The key to find.
 *
 * Returns true if the key is stored in the map.
 */
bool hashmap_contains(struct hashmap *this, struct hkey *key) {
    uint32_t hashed = hkey_hash(key->data, key->length);
    size_t bucket = hashed % this->capacity;
    struct hentry *entry = this->entries[bucket];

    while (entry) {
        if (hkey_equals(entry->key, key)) {
            return true;
        }
        entry = entry->chain;
    }

    return false;
}

/* Remove the value stored under the key. The value memory is not released by
 * this function. The caller should free the value if needed.
 *
 * this - The hashmap from which to remove the key/value pair.
 * key  - The key whose value should be discarded.
 *
 * Returns the stored value or null if the key didn't exist.
 */
void *hashmap_remove(struct hashmap *this, struct hkey *key) {
    uint32_t hashed = hkey_hash(key->data, key->length);
    size_t bucket = hashed % this->capacity;
    struct hentry *entry = this->entries[bucket];
    struct hentry *previous = NULL;

    while (entry) {
        if (hkey_equals(entry->key, key)) {
            if (previous) {
                previous->chain = entry->chain;
            }

            if (entry == this->entries[bucket]) {
                this->entries[bucket] = entry->chain;
            }

            if (entry == this->head) {
                this->head = entry->next;
            }

            if (entry == this->tail) {
                this->tail = entry->prev;
            }

            if (entry->prev) {
                entry->prev->next = entry->next;
            }

            if (entry->next) {
                entry->next->prev = entry->prev;
            }

            this->size--;

            void *evicted = entry->value;
            hentry_destroy(entry);
            return evicted;
        }

        previous = entry;
        entry = entry->chain;
    }

    return NULL;
}

/* Combine two hashmaps into one. If a key exists in both maps, the `other`
 * hashmap's value takes precedence. The value stored at an overwritten key
 * must be freed by the caller, as needed.
 *
 * this  - The hashmap to merge into.
 * other - The map whose keys should be copied into `this`.
 *
 * Returns true if the merge succeeded, false if memory allocation failed.
 */
bool hashmap_merge(struct hashmap *this, struct hashmap *other) {
    size_t total = this->size + other->size;
    if (this->capacity < total) {
        if (!hashmap_resize(this, total)) {
            return false;
        }
    }

    struct hentry *entry = other->head;
    while (entry) {
        if (!hashmap_set(this, entry->key, entry->value) && errno) {
            return false;
        }
        entry = entry->next;
    }

    return true;
}

/* Create an external iterator over the hashmap's key/value entries. The
 * entries are returned in key insertion order. The caller must free the
 * iterator's memory when iteration is complete.
 *
 * this - The hashmap to iterate through.
 *
 * Examples
 *
 *   struct iterator *entries = hashmap_iterator(map);
 *   while (entries->next(entries)) {
 *       struct hentry *entry = entries->current;
 *       printf("%s => %x\n", entry->key, entry->value);
 *   }
 *   entries->destroy(entries);
 *
 * Returns an iterator over the map's entries or null if allocation failed.
 */
struct iterator *hashmap_iterator(struct hashmap *this) {
    return iterator_create(this->head, hashmap_next_entry);
}

/* Private: Advance the iterator to the next entry in the map. This is the
 * implementation of the iter->next() function pointer for hashmaps.
 *
 * The entries are returned in key insertion order.
 *
 * this - The iterator to advance.
 *
 * Returns the next entry or null when the end is reached.
 */
void *hashmap_next_entry(struct iterator *this) {
    struct hentry *entry = this->iterable;

    bool first = !this->current && this->index == 0;

    if (entry) {
        this->iterable = entry->next;
        this->current = entry;
        if (!first) {
            this->index++;
        }
    } else {
        this->current = NULL;
    }

    return this->current;
}

/* Private: Allocate additional memory to accomodate a hash table with more
 * buckets and a reduced load factor.
 *
 * This is triggered when the current load factor reaches above 75%. That is,
 * when the number of entries consumes most of the available buckets, it's
 * time to rehash to avoid excessive chaining within each bucket.
 *
 * The existing entries are rehashed into the new buckets, and the previous
 * bucket memory is released.
 *
 * this     - The hashmap to rehash.
 * capacity - The new number of buckets.
 *
 * Returns false if memory allocation failed, true for success.
 */
bool hashmap_resize(struct hashmap *this, size_t capacity) {
    struct hentry **entries = calloc(capacity, sizeof(struct hentry *));
    if (!entries) {
        return false;
    }

    struct hentry *entry = this->head;
    while (entry) {
        uint32_t hashed = hkey_hash(entry->key->data, entry->key->length);
        size_t bucket = hashed % capacity;

        struct hentry *start = entries[bucket];
        entries[bucket] = entry;
        entry->chain = start;

        entry = entry->next;
    }

    free(this->entries);
    this->entries = entries;
    this->capacity = capacity;

    return true;
}

/* Private: Allocate memory for a new key/value entry pair. The entry must be
 * freed with hentry_destroy.
 *
 * key   - The key to use to lookup the value in the map.
 * value - The value to store in the map.
 *
 * Returns the entry or null if allocation failed.
 */
struct hentry *hentry_create(struct hkey *key, void *value) {
    struct hentry *this = calloc(1, sizeof(struct hentry));
    if (!this) {
        return NULL;
    }

    this->key = hkey_create(key->data, key->length);
    if (!this->key) {
        hentry_destroy(this);
        return NULL;
    }

    this->value = value;
    this->chain = NULL;
    this->prev = NULL;
    this->next = NULL;

    return this;
}

/* Private: Free the memory associated with the entry. The key is freed along
 * with the entry, but the value stored in the entry is not. The caller must
 * free the entry value.
 *
 * entry - The hash entry to free.
 *
 * Returns nothing.
 */
void hentry_destroy(struct hentry *this) {
    hkey_destroy(this->key);
    this->key = NULL;
    this->value = NULL;
    this->chain = NULL;
    this->prev = NULL;
    this->next = NULL;
    free(this);
}

/* Private: Allocate memory for a hash key. The key must be freed with
 * hkey_destroy.
 *
 * data   - The bytes to hash.
 * length - The number of bytes in data.
 *
 * Returns the key or null if allocation failed.
 */
struct hkey *hkey_create(const void *data, size_t length) {
    struct hkey *this = calloc(1, sizeof(struct hkey));
    if (!this) {
        return NULL;
    }

    this->data = malloc(length);
    if (!this->data) {
        hkey_destroy(this);
        return NULL;
    }

    this->length = length;
    memcpy(this->data, data, length);
    return this;
}

/* Private: Free the memory associated with the key.
 *
 * this - The key memory to free.
 *
 * Returns nothing.
 */
void hkey_destroy(struct hkey *this) {
    free(this->data);
    this->data = NULL;
    this->length = 0;
    free(this);
}

/* Private: Compare two keys for equality. This determines if a match is found
 * in the entry list for a key/value pair.
 *
 * a - The first key.
 * b - Another key.
 *
 * Returns true if the keys match.
 */
bool hkey_equals(struct hkey *a, struct hkey *b) {
    return (a->length == b->length) && memcmp(a->data, b->data, a->length) == 0;
}

/* Private: Computes the FNV-1a hash of the key data.
 *
 * data   - The bytes to hash.
 * length - The number of bytes in data.
 *
 * Returns the numerical hash of the input bytes.
 */
uint32_t hkey_hash(const void *data, size_t length) {
    const unsigned char *bytes = data;
    uint32_t hash = 2166136261;

    for (size_t i = 0; i < length; i++) {
        hash = (hash ^ bytes[i]) * 16777619;
    }
    return hash;
}
