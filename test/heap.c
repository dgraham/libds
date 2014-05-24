#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "heap.h"

void assert(bool success, const char *message);
int compare_nodes(const void *a, const void *b);
void test_create(void);
void test_push(void);
void test_pop(void);
void test_clear(void);
void test_clone(void);
void test_merge(void);

void assert(bool success, const char *message) {
    if (!success) {
        fprintf(stderr, "\t[heap] failed: %s\n", message);
        exit(1);
    }
}

int compare_nodes(const void *a, const void *b) {
    return strcmp(a, b);
}

void test_create() {
    struct heap *heap = heap_create(compare_nodes);

    assert(heap->nodes != NULL, "has memory for nodes");
    assert(heap->capacity > 0, "has initial capacity for nodes");
    assert(heap->comparator == compare_nodes, "has a comparator function");
    assert(heap->size == 0, "heap is empty");

    heap_destroy(heap);
}

void test_push() {
    struct heap *heap = heap_create(compare_nodes);

    char *a = "test 1";
    char *b = "test 2";

    assert(heap_push(heap, b), "max item pushed");
    assert(heap->size == 1, "incremented size");
    assert(heap->nodes[0] == b, "root node is max item");

    assert(heap_push(heap, a), "min item pushed");
    assert(heap->size == 2, "incremented size");
    assert(heap->nodes[0] == a, "root node is min item");
    assert(heap->nodes[1] == b, "root node + 1 is max item");

    heap_destroy(heap);
}

void test_pop() {
    struct heap *heap = heap_create(compare_nodes);

    char *a = "test 1";
    char *b = "test 2";
    heap_push(heap, b);
    heap_push(heap, a);

    assert(heap_pop(heap) == a, "min item popped");
    assert(heap->size == 1, "decremented size");
    assert(heap->nodes[0] == b, "root node is last item");

    assert(heap_pop(heap) == b, "last item popped");
    assert(heap->size == 0, "decremented size");
    assert(heap->nodes[0] == NULL, "root node is null");

    assert(heap_pop(heap) == NULL, "popped null when empty");
    assert(heap->size == 0, "size still zero when empty");

    heap_destroy(heap);
}

void test_clear() {
    struct heap *heap = heap_create(compare_nodes);

    char *a = "test 1";
    char *b = "test 2";
    heap_push(heap, a);
    heap_push(heap, b);

    heap_clear(heap);

    assert(heap->nodes[0] == NULL, "root node is null");
    assert(heap->size == 0, "heap is empty");
    assert(heap->capacity > 0, "still has capacity");

    heap_destroy(heap);
}

void test_clone() {
    struct heap *heap = heap_create(compare_nodes);

    char *a = "test 1";
    char *b = "test 2";
    heap_push(heap, a);
    heap_push(heap, b);

    struct heap *clone = heap_clone(heap);
    assert(clone != NULL, "clone allocated memory");
    assert(clone != heap, "clone is not same as original");
    assert(clone->size == 2, "size matches source");

    assert(heap_pop(clone) == a, "original heap unchanged by clone");
    assert(clone->size == 1, "size of clone changed");
    assert(heap->size == 2, "size of original did not change");

    heap_destroy(heap);
    heap_destroy(clone);
}

void test_merge() {
    struct heap *heap1 = heap_create(compare_nodes);
    struct heap *heap2 = heap_create(compare_nodes);

    char *a = "test 1";
    char *b = "test 2";
    heap_push(heap1, a);
    heap_push(heap1, b);

    char *c = "test 3";
    char *d = "test 4";
    heap_push(heap2, c);
    heap_push(heap2, d);

    assert(heap_merge(heap1, heap2), "merged");
    assert(heap1->size == 4, "incremented size");
    assert(heap2->size == 2, "source heap size unchanged");

    assert(heap_pop(heap1) == a, "popped first");
    assert(heap_pop(heap1) == b, "popped second");
    assert(heap_pop(heap1) == c, "popped third");
    assert(heap_pop(heap1) == d, "popped fourth");
    assert(heap_pop(heap1) == NULL, "heap1 is empty");

    assert(heap_pop(heap2) == c, "popped first");
    assert(heap_pop(heap2) == d, "popped second");
    assert(heap_pop(heap2) == NULL, "heap2 is empty");

    heap_destroy(heap1);
    heap_destroy(heap2);
}

int main() {
    test_create();
    test_push();
    test_pop();
    test_clear();
    test_clone();
    test_merge();

    return 0;
}
