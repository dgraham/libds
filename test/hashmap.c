#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include "hashmap.h"

void test_create(void);
void test_get(void);
void test_set(void);
void test_contains(void);
void test_iterator(void);
void test_remove(void);
void test_merge(void);

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

    char *a = "item 1";
    char *b = "item 2";

    assert(hashmap_get(map, &key) == NULL);

    hashmap_set(map, &key, a);
    assert(hashmap_get(map, &key) == a);

    hashmap_set(map, &key, b);
    assert(hashmap_get(map, &key) == b);

    int id2 = 12;
    struct hkey key2 = {&id2, sizeof(id2)};

    hashmap_set(map, &key2, a);
    assert(hashmap_get(map, &key) == b);
    assert(hashmap_get(map, &key2) == a);

    hashmap_destroy(map);
}

void test_set() {
    struct hashmap *map = hashmap_create();

    int id = 42;
    struct hkey key = {&id, sizeof(id)};

    char *a = "item 1";
    char *b = "item 2";

    errno = 1;
    assert(hashmap_set(map, &key, a) == NULL);
    assert(errno == 0);
    assert(map->size == 1);
    assert(map->head != NULL);
    assert(map->tail != NULL);
    assert(map->head == map->tail);
    assert(map->tail->next == NULL);

    errno = 1;
    assert(hashmap_set(map, &key, b) == a);
    assert(errno == 0);
    assert(map->size == 1);

    int id2 = 42;
    struct hkey key2 = {&id2, sizeof(id2)};
    errno = 1;
    assert(hashmap_set(map, &key2, a) == b);
    assert(errno == 0);
    assert(map->size == 1);

    int id3 = 12;
    struct hkey key3 = {&id3, sizeof(id3)};
    errno = 1;
    assert(hashmap_set(map, &key3, a) == NULL);
    assert(errno == 0);
    assert(map->size == 2);
    assert(map->head != map->tail);
    assert(map->tail->next == NULL);
    assert(map->head->next == map->tail);

    hashmap_destroy(map);
}

void test_contains() {
    struct hashmap *map = hashmap_create();

    int id = 42;
    struct hkey key = {&id, sizeof(id)};

    char *a = "item 1";

    assert(!hashmap_contains(map, &key));

    hashmap_set(map, &key, a);
    assert(hashmap_contains(map, &key));

    hashmap_set(map, &key, NULL);
    assert(hashmap_contains(map, &key));

    hashmap_destroy(map);
}

void test_iterator() {
    struct hashmap *map = hashmap_create();

    int id1 = 42;
    struct hkey key1 = {&id1, sizeof(id1)};

    int id2 = 1000;
    struct hkey key2 = {&id2, sizeof(id2)};

    int id3 = 500;
    struct hkey key3 = {&id3, sizeof(id3)};

    char *a = "test 1";
    char *b = "test 2";
    char *c = "test 3";
    hashmap_set(map, &key1, a);
    hashmap_set(map, &key2, b);
    hashmap_set(map, &key3, c);

    struct iterator *entries = hashmap_iterator(map);
    assert(entries->destroy != NULL);
    assert(entries->current == NULL);
    assert(entries->index == 0);

    struct hentry *entry1 = entries->next(entries);
    assert(entry1->value == a);
    assert(entries->current == entry1);
    assert(entries->index == 0);

    struct hentry *entry2 = entries->next(entries);
    assert(entry2->value == b);
    assert(entries->current == entry2);
    assert(entries->index == 1);

    struct hentry *entry3 = entries->next(entries);
    assert(entry3->value == c);
    assert(entries->current == entry3);
    assert(entries->index == 2);

    struct hentry *entry4 = entries->next(entries);
    assert(entry4 == NULL);
    assert(entries->current == NULL);
    assert(entries->index == 2);

    entries->destroy(entries);
    hashmap_destroy(map);
}

void test_remove() {
    struct hashmap *map = hashmap_create();

    char *a = "item 1";
    char *b = "item 2";
    char *c = "item 3";

    int id = 42;
    struct hkey key = {&id, sizeof(id)};
    hashmap_set(map, &key, a);

    int id2 = 22;
    struct hkey key2 = {&id2, sizeof(id2)};
    hashmap_set(map, &key2, b);

    int id3 = 12;
    struct hkey key3 = {&id3, sizeof(id3)};
    hashmap_set(map, &key3, c);

    assert(hashmap_remove(map, &key) == a);
    assert(map->size == 2);
    assert(map->head != NULL);
    assert(map->tail != NULL);
    assert(map->head != map->tail);

    assert(hashmap_remove(map, &key2) == b);
    assert(map->size == 1);
    assert(map->head != NULL);
    assert(map->tail != NULL);
    assert(map->head == map->tail);

    assert(hashmap_remove(map, &key2) == NULL);
    assert(map->size == 1);
    assert(map->head != NULL);
    assert(map->tail != NULL);
    assert(map->head == map->tail);

    assert(hashmap_remove(map, &key3) == c);
    assert(map->size == 0);
    assert(map->head == NULL);
    assert(map->tail == NULL);

    hashmap_destroy(map);
}

void test_merge() {
    struct hashmap *map1 = hashmap_create();
    struct hashmap *map2 = hashmap_create();

    char *a1 = "item a 1";
    char *a2 = "item a 2";
    char *b = "item 2";
    char *c = "item 3";

    int id = 42;
    struct hkey key = {&id, sizeof(id)};
    hashmap_set(map1, &key, a1);
    hashmap_set(map2, &key, a2);

    int id2 = 22;
    struct hkey key2 = {&id2, sizeof(id2)};
    hashmap_set(map2, &key2, b);

    int id3 = 12;
    struct hkey key3 = {&id3, sizeof(id3)};
    hashmap_set(map2, &key3, c);

    assert(hashmap_merge(map1, map2));
    assert(map1->size == 3);
    assert(map2->size == 3);
    assert(hashmap_get(map2, &key) == a2);
    assert(hashmap_get(map1, &key) == a2);
    assert(hashmap_get(map1, &key2) == b);
    assert(hashmap_get(map1, &key3) == c);

    hashmap_destroy(map1);
    hashmap_destroy(map2);
}

int main() {
    test_create();
    test_get();
    test_set();
    test_contains();
    test_iterator();
    test_remove();
    test_merge();

    return 0;
}
