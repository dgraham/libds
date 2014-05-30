#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "hashmap.h"

void test_create(void);
void test_get(void);

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
    struct hkey key = {&id, sizeof(int)};
    assert(hashmap_get(map, &key) == NULL);

    hashmap_destroy(map);
}

int main() {
    test_create();
    test_get();

    return 0;
}
