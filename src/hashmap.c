#include <string.h>
#include "hashmap.h"

bool hashmap_resize(struct hashmap *this, size_t capacity);

struct hentry *hentry_create(struct hkey *key, void *value);
void hentry_destroy(struct hentry *entry);

struct hkey *hkey_create(const void *data, size_t length);
void hkey_destroy(struct hkey *key);
bool hkey_equals(struct hkey *a, struct hkey *b);
uint32_t hkey_hash(const void *data, size_t length);

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

    memset(this->entries, 0, this->capacity * sizeof(struct entry *));

    this->head = NULL;
    this->tail = NULL;
    this->size = 0;
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
    this->key   = NULL;
    this->value = NULL;
    this->chain = NULL;
    this->prev  = NULL;
    this->next  = NULL;
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
