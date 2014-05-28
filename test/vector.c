#include <stdlib.h>
#include <stdio.h>
#include "vector.h"

void assert(bool success, const char *message);
void test_create(void);
void test_push(void);
void test_pop(void);

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

int main() {
    test_create();
    test_push();
    test_pop();

    return 0;
}
