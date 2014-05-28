#include <stdlib.h>
#include <stdio.h>
#include "vector.h"

void assert(bool success, const char *message);
void test_create(void);
void test_push(void);
void test_pop(void);
void test_clear(void);
void test_clone(void);

void assert(bool success, const char *message) {
    if (!success) {
        fprintf(stderr, "\t[vector] failed: %s\n", message);
        exit(1);
    }
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

int main() {
    test_create();
    test_push();
    test_pop();
    test_clear();
    test_clone();

    return 0;
}
