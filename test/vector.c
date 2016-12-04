#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "vector.h"

int compare_items(const void *a, const void *b);
void test_create(void);
void test_push(void);
void test_pop(void);
void test_clear(void);
void test_clone(void);
void test_concat(void);
void test_unshift(void);
void test_shift(void);
void test_sort(void);
void test_iterator(void);
void test_get(void);
void test_set(void);
void test_slice(void);
void test_remove(void);
void test_insert(void);

int compare_items(const void *a, const void *b) {
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wcast-qual"
    const char **a2 = (const char **) a;
    const char **b2 = (const char **) b;
    #pragma clang diagnostic pop
    return strcmp(*a2, *b2);
}

void test_create() {
    struct vector *vector = vector_create();

    assert(vector->length == 0);
    assert(vector->capacity > 0);
    assert(vector->items != NULL);

    vector_destroy(vector);
}

void test_push() {
    struct vector *vector = vector_create();

    char *a = "item1";
    assert(vector_push(vector, a));
    assert(vector->length == 1);
    assert(vector->items[0] == a);

    char *b = "item2";
    assert(vector_push(vector, b));
    assert(vector->length == 2);
    assert(vector->items[1] == b);

    vector_destroy(vector);
}

void test_pop() {
    struct vector *vector = vector_create();

    char *a = "item1";
    char *b = "item2";
    vector_push(vector, a);
    vector_push(vector, b);

    assert(vector_pop(vector) == b);
    assert(vector->length == 1);
    assert(vector->items[1] == NULL);

    assert(vector_pop(vector) == a);
    assert(vector->length == 0);
    assert(vector->items[0] == NULL);

    vector_destroy(vector);
}

void test_clear() {
    struct vector *vector = vector_create();

    char *a = "item1";
    char *b = "item2";
    vector_push(vector, a);
    vector_push(vector, b);

    vector_clear(vector);

    assert(vector->length == 0);
    assert(vector->items[0] == NULL);
    assert(vector->items[1] == NULL);
    assert(vector->capacity > 0);

    vector_destroy(vector);
}

void test_clone() {
    struct vector *vector = vector_create();

    char *a = "item1";
    char *b = "item2";
    vector_push(vector, a);
    vector_push(vector, b);

    struct vector *clone = vector_clone(vector);
    assert(clone != NULL);
    assert(clone != vector);
    assert(clone->length == 2);

    assert(vector_pop(clone) == b);
    assert(clone->length == 1);
    assert(vector->length == 2);

    vector_destroy(vector);
    vector_destroy(clone);
}

void test_concat() {
    struct vector *list1 = vector_create();
    struct vector *list2 = vector_create();

    char *item1 = "test1";
    char *item2 = "test2";
    vector_push(list1, item1);
    vector_push(list1, item2);

    char *item3 = "test3";
    char *item4 = "test4";
    vector_push(list2, item3);
    vector_push(list2, item4);

    assert(vector_concat(list1, list2));
    assert(list1->length == 4);
    assert(list2->length == 2);

    assert(vector_pop(list1) == item4);
    assert(vector_pop(list1) == item3);
    assert(vector_pop(list1) == item2);
    assert(vector_pop(list1) == item1);
    assert(vector_pop(list1) == NULL);

    assert(vector_pop(list2) == item4);
    assert(vector_pop(list2) == item3);
    assert(vector_pop(list2) == NULL);

    vector_destroy(list1);
    vector_destroy(list2);
}

void test_unshift() {
    struct vector *vector = vector_create();

    char *a = "item1";
    assert(vector_unshift(vector, a));
    assert(vector->length == 1);

    char *b = "item2";
    assert(vector_unshift(vector, b));
    assert(vector->length == 2);

    assert(vector_pop(vector) == a);
    assert(vector_pop(vector) == b);

    vector_destroy(vector);
}

void test_shift() {
    struct vector *vector = vector_create();

    char *a = "item1";
    char *b = "item2";

    vector_unshift(vector, a);
    vector_unshift(vector, b);

    assert(vector_shift(vector) == b);
    assert(vector->length == 1);

    assert(vector_shift(vector) == a);
    assert(vector->length == 0);

    assert(vector_shift(vector) == NULL);
    assert(vector->length == 0);

    vector_destroy(vector);
}

void test_sort() {
    struct vector *vector = vector_create();

    char *a = "item1";
    char *b = "item2";
    char *c = "item3";

    vector_push(vector, b);
    vector_push(vector, c);
    vector_push(vector, a);

    vector_sort(vector, compare_items);

    assert(vector_shift(vector) == a);
    assert(vector_shift(vector) == b);
    assert(vector_shift(vector) == c);

    vector_destroy(vector);
}

void test_iterator() {
    struct vector *vector = vector_create();

    char *item1 = "test1";
    char *item2 = "test2";
    vector_push(vector, item1);
    vector_push(vector, item2);

    struct iterator *items = vector_iterator(vector);
    assert(items->destroy != NULL);
    assert(items->current == NULL);
    assert(items->index == 0);

    void *first = items->next(items);
    assert(first == item1);
    assert(items->current == item1);
    assert(items->index == 1);

    void *second = items->next(items);
    assert(second == item2);
    assert(items->current == item2);
    assert(items->index == 2);

    void *third = items->next(items);
    assert(third == NULL);
    assert(items->current == NULL);
    assert(items->index == 2);

    items->destroy(items);
    vector_destroy(vector);
}

void test_get() {
    struct vector *vector = vector_create();

    assert(vector_get(vector, 0) == NULL);

    char *a = "item1";
    vector_push(vector, a);

    assert(vector_get(vector, 0) == a);
    assert(vector_get(vector, 1) == NULL);

    vector_destroy(vector);
}

void test_set() {
    struct vector *vector = vector_create();

    char *a = "item1";
    char *b = "item2";

    assert(vector_set(vector, 0, a) == NULL);

    vector_push(vector, a);

    assert(vector_set(vector, 1, b) == NULL);
    assert(vector_set(vector, 0, b) == a);
    assert(vector_get(vector, 0) == b);

    vector_destroy(vector);
}

void test_slice() {
    struct vector *vector = vector_create();
    struct vector *slice = NULL;

    char *a = "item1";
    char *b = "item2";

    slice = vector_slice(vector, 0, 1);
    assert(slice != NULL);
    assert(slice->length == 0);
    vector_destroy(slice);

    vector_push(vector, a);

    slice = vector_slice(vector, 0, 0);
    assert(slice != NULL);
    assert(slice->length == 0);
    vector_destroy(slice);

    slice = vector_slice(vector, 1, 1);
    assert(slice != NULL);
    assert(slice->length == 0);
    vector_destroy(slice);

    slice = vector_slice(vector, 0, 2);
    assert(slice != NULL);
    assert(slice->length == 1);
    assert(vector_get(slice, 0) == a);
    vector_destroy(slice);

    vector_push(vector, b);

    slice = vector_slice(vector, 1, 1);
    assert(slice != NULL);
    assert(slice->length == 1);
    assert(vector_get(slice, 0) == b);
    vector_destroy(slice);

    slice = vector_slice(vector, 0, 2);
    assert(slice != NULL);
    assert(slice->length == 2);
    assert(vector_get(slice, 0) == a);
    assert(vector_get(slice, 1) == b);
    vector_destroy(slice);

    vector_destroy(vector);
}

void test_remove() {
    struct vector *vector = vector_create();

    char *a = "item1";
    char *b = "item2";

    assert(vector_remove(vector, 0) == NULL);

    vector_push(vector, a);

    assert(vector_remove(vector, 1) == NULL);

    assert(vector_remove(vector, 0) == a);
    assert(vector->length == 0);
    assert(vector->items[0] == NULL);

    vector_push(vector, a);
    vector_push(vector, b);

    assert(vector_remove(vector, 0) == a);
    assert(vector->length == 1);
    assert(vector->items[0] == b);
    assert(vector->items[1] == NULL);

    vector_clear(vector);
    vector_push(vector, a);
    vector_push(vector, b);

    assert(vector_remove(vector, 1) == b);
    assert(vector->length == 1);
    assert(vector->items[0] == a);
    assert(vector->items[1] == NULL);

    vector_destroy(vector);
}

void test_insert() {
    struct vector *vector = vector_create();

    char *a = "item1";
    char *b = "item2";
    char *c = "item3";

    assert(!vector_insert(vector, 0, a));

    vector_push(vector, a);

    assert(!vector_insert(vector, 1, a));

    assert(vector_insert(vector, 0, b));
    assert(vector->length == 2);
    assert(vector->items[0] == b);
    assert(vector->items[1] == a);

    assert(vector_insert(vector, 1, c));
    assert(vector->length == 3);
    assert(vector->items[0] == b);
    assert(vector->items[1] == c);
    assert(vector->items[2] == a);

    vector_destroy(vector);
}

int main() {
    test_create();
    test_push();
    test_pop();
    test_clear();
    test_clone();
    test_concat();
    test_unshift();
    test_shift();
    test_sort();
    test_iterator();
    test_get();
    test_set();
    test_slice();
    test_remove();
    test_insert();

    return 0;
}
