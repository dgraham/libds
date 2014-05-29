#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "vector.h"

void assert(bool success, const char *message);
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

void assert(bool success, const char *message) {
    if (!success) {
        fprintf(stderr, "\t[vector] failed: %s\n", message);
        exit(1);
    }
}

int compare_items(const void *a, const void *b) {
    const char **a2 = (const char **) a;
    const char **b2 = (const char **) b;
    return strcmp(*a2, *b2);
}

void test_create() {
    struct vector *vector = vector_create();

    assert(vector->length == 0, "vector is empty");
    assert(vector->capacity > 0, "vector has item capacity");
    assert(vector->items != NULL, "vector has items");

    vector_destroy(vector);
}

void test_push() {
    struct vector *vector = vector_create();

    char *a = "item1";
    assert(vector_push(vector, a), "first item accepted");
    assert(vector->length == 1, "length is incremented");
    assert(vector->items[0] == a, "first pointer stored");

    char *b = "item2";
    assert(vector_push(vector, b), "second item accepted");
    assert(vector->length == 2, "length is incremented");
    assert(vector->items[1] == b, "second pointer stored");

    vector_destroy(vector);
}

void test_pop() {
    struct vector *vector = vector_create();

    char *a = "item1";
    char *b = "item2";
    vector_push(vector, a);
    vector_push(vector, b);

    assert(vector_pop(vector) == b, "removed last item");
    assert(vector->length == 1, "length is decremented");
    assert(vector->items[1] == NULL, "cleared item pointer");

    assert(vector_pop(vector) == a, "removed first item");
    assert(vector->length == 0, "vector is empty");
    assert(vector->items[0] == NULL, "cleared item pointer");

    vector_destroy(vector);
}

void test_clear() {
    struct vector *vector = vector_create();

    char *a = "item1";
    char *b = "item2";
    vector_push(vector, a);
    vector_push(vector, b);

    vector_clear(vector);

    assert(vector->length == 0, "vector is empty");
    assert(vector->items[0] == NULL, "cleared first item pointer");
    assert(vector->items[1] == NULL, "cleared last item pointer");
    assert(vector->capacity > 0, "still has capacity");

    vector_destroy(vector);
}

void test_clone() {
    struct vector *vector = vector_create();

    char *a = "item1";
    char *b = "item2";
    vector_push(vector, a);
    vector_push(vector, b);

    struct vector *clone = vector_clone(vector);
    assert(clone != NULL, "clone allocated memory");
    assert(clone != vector, "clone is not same as original");
    assert(clone->length == 2, "length matches source");

    assert(vector_pop(clone) == b, "original vector unchanged by clone");
    assert(clone->length == 1, "length of clone changed");
    assert(vector->length == 2, "length of original did not change");

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

    assert(vector_concat(list1, list2), "concatenation succeeded");
    assert(list1->length == 4, "incremented length");
    assert(list2->length == 2, "source list length unchanged");

    assert(vector_pop(list1) == item4, "popped item four");
    assert(vector_pop(list1) == item3, "popped item three");
    assert(vector_pop(list1) == item2, "popped item two");
    assert(vector_pop(list1) == item1, "popped item one");
    assert(vector_pop(list1) == NULL, "vector is empty");

    assert(vector_pop(list2) == item4, "popped last item");
    assert(vector_pop(list2) == item3, "popped first item");
    assert(vector_pop(list2) == NULL, "vector is empty");

    vector_destroy(list1);
    vector_destroy(list2);
}

void test_unshift() {
    struct vector *vector = vector_create();

    char *a = "item1";
    assert(vector_unshift(vector, a), "unshifted first item");
    assert(vector->length == 1, "incremented length");

    char *b = "item2";
    assert(vector_unshift(vector, b), "unshifted second item");
    assert(vector->length == 2, "incremented length");

    assert(vector_pop(vector) == a, "popped first item");
    assert(vector_pop(vector) == b, "popped second item");

    vector_destroy(vector);
}

void test_shift() {
    struct vector *vector = vector_create();

    char *a = "item1";
    char *b = "item2";

    vector_unshift(vector, a);
    vector_unshift(vector, b);

    assert(vector_shift(vector) == b, "shifted second item");
    assert(vector->length == 1, "decremented length");

    assert(vector_shift(vector) == a, "shifted first item");
    assert(vector->length == 0, "decremented length");

    assert(vector_shift(vector) == NULL, "shifted null item");
    assert(vector->length == 0, "does not decrement length");

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

    assert(vector_shift(vector) == a, "first item in place");
    assert(vector_shift(vector) == b, "second item in place");
    assert(vector_shift(vector) == c, "third item in place");

    vector_destroy(vector);
}

void test_iterator() {
    struct vector *vector = vector_create();

    char *item1 = "test1";
    char *item2 = "test2";
    vector_push(vector, item1);
    vector_push(vector, item2);

    struct iterator *items = vector_iterator(vector);
    assert(items->destroy != NULL, "has a destructor function");
    assert(items->current == NULL, "starts with null item");
    assert(items->index == 0, "starts at index 0");

    void *first = items->next(items);
    assert(first == item1, "returns first item");
    assert(items->current == item1, "stores current item");
    assert(items->index == 1, "increments index");

    void *second = items->next(items);
    assert(second == item2, "returns second item");
    assert(items->current == item2, "stores current item");
    assert(items->index == 2, "increments index");

    void *third = items->next(items);
    assert(third == NULL, "returns null when iteration is complete");
    assert(items->current == NULL, "current item is null");
    assert(items->index == 2, "does not increment index");

    items->destroy(items);
    vector_destroy(vector);
}

void test_get() {
    struct vector *vector = vector_create();

    assert(vector_get(vector, 0) == NULL, "empty vector returns null");

    char *a = "item1";
    vector_push(vector, a);

    assert(vector_get(vector, 0) == a, "get returns item pointer");
    assert(vector_get(vector, 1) == NULL, "out of bounds returns null");

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

    return 0;
}
