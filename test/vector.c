#include <stdlib.h>
#include <stdio.h>
#include "vector.h"

void assert(bool success, const char *message);
void test_create(void);

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

int main() {
    test_create();

    return 0;
}
