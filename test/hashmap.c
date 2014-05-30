#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "hashmap.h"

void test_create(void);

void test_create() {
    struct hashmap *map = hashmap_create();

    assert(map->head == NULL);
    assert(map->tail == NULL);
    assert(map->size == 0);
    assert(map->capacity > 0);

    hashmap_destroy(map);
}

int main() {
    test_create();

    return 0;
}
