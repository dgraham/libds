#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "hashmap.h"

void test_create(void);
void test_get(void);
void test_set(void);

void test_create() {
    struct hashmap *map = hashmap_create();

    assert(map->head == NULL);
    assert(map->tail == NULL);
    assert(map->size == 0);
    assert(map->capacity > 0);

    hashmap_destroy(map);
}

void test_get() {
    struct hashmap *map = hashmap_create();

    int id = 42;
    struct hkey key = {&id, sizeof(id)};
    assert(hashmap_get(map, &key) == NULL);

    hashmap_destroy(map);
}

void test_set() {
    struct hashmap *map = hashmap_create();

    int id = 42;
    struct hkey key = {&id, sizeof(id)};

    char *a = "item 1";
    char *b = "item 2";

    assert(hashmap_set(map, &key, a) == NULL);
    assert(map->size == 1);
    assert(map->head != NULL);
    assert(map->tail != NULL);
    assert(map->head == map->tail);
    assert(map->tail->next == NULL);

    assert(hashmap_set(map, &key, b) == a);
    assert(map->size == 1);

    int id2 = 42;
    struct hkey key2 = {&id2, sizeof(id2)};
    assert(hashmap_set(map, &key2, a) == b);
    assert(map->size == 1);

    int id3 = 12;
    struct hkey key3 = {&id3, sizeof(id3)};
    assert(hashmap_set(map, &key3, a) == NULL);
    assert(map->size == 2);
    assert(map->head != map->tail);
    assert(map->tail->next == NULL);
    assert(map->head->next == map->tail);

    hashmap_destroy(map);
}

int main() {
    test_create();
    test_get();
    test_set();

    return 0;
}
