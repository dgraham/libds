#ifndef HASHMAP_H
#define HASHMAP_H

#include "iterator.h"
#include <stdbool.h>
#include <stdlib.h>

struct hkey {
    void *data;
    size_t length;
};

struct hentry {
    struct hentry *prev;
    struct hentry *next;
    struct hentry *chain;
    struct hkey *key;
    void *value;
};

struct hashmap {
    struct hentry **entries;
    struct hentry *head;
    struct hentry *tail;
    size_t capacity;
    size_t size;
};

struct hashmap *hashmap_create(void);

void hashmap_destroy(struct hashmap *this);

struct hashmap *hashmap_clone(struct hashmap *this);

void *hashmap_get(struct hashmap *this, struct hkey *key);

void *hashmap_set(struct hashmap *this, struct hkey *key, void *value);

bool hashmap_contains(struct hashmap *this, struct hkey *key);

void *hashmap_remove(struct hashmap *this, struct hkey *key);

void hashmap_clear(struct hashmap *this);

bool hashmap_merge(struct hashmap *this, struct hashmap *other);

struct iterator *hashmap_iterator(struct hashmap *this);

#endif
