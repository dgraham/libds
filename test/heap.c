#include <stdlib.h>
#include <stdio.h>
#include "heap.h"

void assert(bool success, const char *message);
int compare_nodes(const void *a, const void *b);
void test_create(void);

void assert(bool success, const char *message) {
    if (!success) {
        fprintf(stderr, "\t[heap] failed: %s\n", message);
        exit(1);
    }
}

int compare_nodes(const void *a, const void *b) {
    return a == b ? 0 : -1;
}

void test_create() {
    struct heap *heap = heap_create(compare_nodes);

    assert(heap->nodes != NULL, "has memory for nodes");
    assert(heap->capacity > 0, "has initial capacity for nodes");
    assert(heap->comparator == compare_nodes, "has a comparator function");
    assert(heap->size == 0, "heap is empty");

    heap_destroy(heap);
}

int main() {
    test_create();

    return 0;
}
