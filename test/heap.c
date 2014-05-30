#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "heap.h"

int compare_nodes(const void *a, const void *b);
void test_create(void);
void test_push(void);
void test_pop(void);
void test_clear(void);
void test_clone(void);
void test_merge(void);
void test_iterator(void);

int compare_nodes(const void *a, const void *b) {
    return strcmp(a, b);
}

void test_create() {
    struct heap *heap = heap_create(compare_nodes);

    assert(heap->nodes != NULL);
    assert(heap->capacity > 0);
    assert(heap->comparator == compare_nodes);
    assert(heap->size == 0);

    heap_destroy(heap);
}

void test_push() {
    struct heap *heap = heap_create(compare_nodes);

    char *a = "test 1";
    char *b = "test 2";

    assert(heap_push(heap, b));
    assert(heap->size == 1);
    assert(heap->nodes[0] == b);

    assert(heap_push(heap, a));
    assert(heap->size == 2);
    assert(heap->nodes[0] == a);
    assert(heap->nodes[1] == b);

    heap_destroy(heap);
}

void test_pop() {
    struct heap *heap = heap_create(compare_nodes);

    char *a = "test 1";
    char *b = "test 2";
    heap_push(heap, b);
    heap_push(heap, a);

    assert(heap_pop(heap) == a);
    assert(heap->size == 1);
    assert(heap->nodes[0] == b);

    assert(heap_pop(heap) == b);
    assert(heap->size == 0);
    assert(heap->nodes[0] == NULL);

    assert(heap_pop(heap) == NULL);
    assert(heap->size == 0);

    heap_destroy(heap);
}

void test_clear() {
    struct heap *heap = heap_create(compare_nodes);

    char *a = "test 1";
    char *b = "test 2";
    heap_push(heap, a);
    heap_push(heap, b);

    heap_clear(heap);

    assert(heap->nodes[0] == NULL);
    assert(heap->size == 0);
    assert(heap->capacity > 0);

    heap_destroy(heap);
}

void test_clone() {
    struct heap *heap = heap_create(compare_nodes);

    char *a = "test 1";
    char *b = "test 2";
    heap_push(heap, a);
    heap_push(heap, b);

    struct heap *clone = heap_clone(heap);
    assert(clone != NULL);
    assert(clone != heap);
    assert(clone->size == 2);

    assert(heap_pop(clone) == a);
    assert(clone->size == 1);
    assert(heap->size == 2);

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

    assert(heap_merge(heap1, heap2));
    assert(heap1->size == 4);
    assert(heap2->size == 2);

    assert(heap_pop(heap1) == a);
    assert(heap_pop(heap1) == b);
    assert(heap_pop(heap1) == c);
    assert(heap_pop(heap1) == d);
    assert(heap_pop(heap1) == NULL);

    assert(heap_pop(heap2) == c);
    assert(heap_pop(heap2) == d);
    assert(heap_pop(heap2) == NULL);

    heap_destroy(heap1);
    heap_destroy(heap2);
}

void test_iterator() {
    struct heap *heap = heap_create(compare_nodes);

    char *a = "test 1";
    char *b = "test 2";
    heap_push(heap, b);
    heap_push(heap, a);

    struct iterator *nodes = heap_iterator(heap);
    assert(nodes->destroy != NULL);
    assert(nodes->current == NULL);
    assert(nodes->index == 0);

    assert(nodes->next(nodes) == a);
    assert(nodes->current == a);
    assert(nodes->index == 0);

    assert(nodes->next(nodes) == b);
    assert(nodes->current == b);
    assert(nodes->index == 1);

    assert(nodes->next(nodes) == NULL);
    assert(nodes->current == NULL);
    assert(nodes->index == 1);

    assert(heap->size == 2);
    assert(heap->nodes[0] == a);

    nodes->destroy(nodes);
    heap_destroy(heap);
}

int main() {
    test_create();
    test_push();
    test_pop();
    test_clear();
    test_clone();
    test_merge();
    test_iterator();

    return 0;
}
