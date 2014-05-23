#include <stdlib.h>
#include <stdio.h>
#include "list.h"

void assert(bool success, const char *message);
void test_create(void);
void test_push(void);
void test_pop(void);
void test_unshift(void);
void test_shift(void);
void test_clone(void);
void test_concat(void);
void test_clear(void);
void test_iterator(void);

void assert(bool success, const char *message) {
    if (!success) {
        fprintf(stderr, "\t[list] failed: %s\n", message);
        exit(1);
    }
}

void test_create() {
    struct list *list = list_create();

    assert(list->head == NULL, "head pointer is null");
    assert(list->tail == NULL, "tail pointer is null");
    assert(list->length == 0, "list is empty");

    list_destroy(list);
}

void test_push() {
    struct list *list = list_create();

    char *item1 = "test1";
    assert(list_push(list, item1), "first item accepted");
    assert(list->head != NULL, "head pointer is set");
    assert(list->tail != NULL, "tail pointer is set");
    assert(list->tail == list->head, "tail pointer equals head");
    assert(list->length == 1, "length is incremented");
    assert(list->head->value == item1, "stored item value");
    assert(list->head->prev == NULL, "head item has no previous item");
    assert(list->head->next == NULL, "head item has no next item");

    char *item2 = "test2";
    assert(list_push(list, item2), "second item accepted");
    assert(list->head != NULL, "head pointer is set");
    assert(list->tail != NULL, "tail pointer is set");
    assert(list->tail != list->head, "moved tail pointer");
    assert(list->length == 2, "length is incremented");
    assert(list->tail->value == item2, "stored item value");
    assert(list->head->next == list->tail, "head item points to tail item");
    assert(list->tail->next == NULL, "tail item points to null");
    assert(list->tail->prev == list->head, "tail previous points to head");
    assert(list->head->prev == NULL, "head has no previous node");

    list_destroy(list);
}

void test_pop() {
    struct list *list = list_create();

    char *item1 = "test1";
    char *item2 = "test2";
    list_push(list, item1);
    list_push(list, item2);

    assert(list_pop(list) == item2, "removed last item");
    assert(list->head != NULL, "head pointer is set");
    assert(list->tail != NULL, "tail pointer is set");
    assert(list->tail == list->head, "tail pointer equals head");
    assert(list->length == 1, "length is decremented");
    assert(list->head->value == item1, "stored item value");
    assert(list->head->prev == NULL, "head item has no previous item");
    assert(list->head->next == NULL, "head item has no next item");

    assert(list_pop(list) == item1, "removed first item");
    assert(list->head == NULL, "head pointer is null");
    assert(list->tail == NULL, "tail pointer is null");
    assert(list->length == 0, "list is empty");

    assert(list_pop(list) == NULL, "pop empty list returned null");
    assert(list->length == 0, "empty list length remains zero");

    list_destroy(list);
}

void test_unshift() {
    struct list *list = list_create();

    char *item1 = "test1";
    assert(list_unshift(list, item1), "first item accepted");
    assert(list->head != NULL, "head pointer is set");
    assert(list->tail != NULL, "tail pointer is set");
    assert(list->tail == list->head, "tail pointer equals head");
    assert(list->length == 1, "length is incremented");
    assert(list->head->value == item1, "stored item value");
    assert(list->head->prev == NULL, "head item has no previous item");
    assert(list->head->next == NULL, "head item has no next item");

    char *item2 = "test2";
    assert(list_unshift(list, item2), "second item accepted");
    assert(list->head != NULL, "head pointer is set");
    assert(list->tail != NULL, "tail pointer is set");
    assert(list->tail != list->head, "moved tail pointer");
    assert(list->length == 2, "length is incremented");
    assert(list->tail->value == item1, "first item is now last");
    assert(list->head->value == item2, "second item is now first");
    assert(list->head->next == list->tail, "head item points to tail item");
    assert(list->tail->next == NULL, "tail item points to null");
    assert(list->tail->prev == list->head, "tail previous points to head");
    assert(list->head->prev == NULL, "head has no previous node");

    list_destroy(list);
}

void test_shift() {
    struct list *list = list_create();

    char *item1 = "test1";
    char *item2 = "test2";
    list_push(list, item1);
    list_push(list, item2);

    assert(list_shift(list) == item1, "removed head item");
    assert(list->head != NULL, "head pointer is set");
    assert(list->tail != NULL, "tail pointer is set");
    assert(list->tail == list->head, "tail pointer equals head");
    assert(list->length == 1, "length is decremented");
    assert(list->head->value == item2, "stored item value");
    assert(list->head->prev == NULL, "head item has no previous item");
    assert(list->head->next == NULL, "head item has no next item");

    assert(list_shift(list) == item2, "removed tail item");
    assert(list->head == NULL, "head pointer is null");
    assert(list->tail == NULL, "tail pointer is null");
    assert(list->length == 0, "list is empty");

    assert(list_shift(list) == NULL, "shift empty list returned null");
    assert(list->length == 0, "empty list length remains zero");

    list_destroy(list);
}

void test_clone() {
    struct list *list = list_create();

    char *item1 = "test1";
    char *item2 = "test2";
    list_push(list, item1);
    list_push(list, item2);

    struct list *clone = list_clone(list);
    assert(clone->length == 2, "length matches source");
    assert(clone->head != NULL, "head pointer is set");
    assert(clone->tail != NULL, "tail pointer is set");
    assert(clone->head->value == item1, "stored first item value");
    assert(clone->tail->value == item2, "stored second item value");
    assert(clone->head->prev == NULL, "head item has no previous item");
    assert(clone->head->next == clone->tail, "head item points to tail");

    assert(clone->head != list->head, "head node is unique");
    assert(clone->tail != list->tail, "tail node is unique");

    assert(list_pop(list) == item2, "original list unchanged by clone");
    assert(list->length == 1, "length of original changed");
    assert(clone->length == 2, "length of clone did not change");

    list_destroy(list);
}

void test_concat() {
    struct list *list1 = list_create();
    struct list *list2 = list_create();

    char *item1 = "test1";
    char *item2 = "test2";
    list_push(list1, item1);
    list_push(list1, item2);

    char *item3 = "test3";
    char *item4 = "test4";
    list_push(list2, item3);
    list_push(list2, item4);

    assert(list_concat(list1, list2), "concatenation succeeded");
    assert(list1->length == 4, "incremented length");
    assert(list2->length == 2, "source list length unchanged");

    list_push(list1, item4);
    assert(list2->tail->next == NULL, "push only adds to one list");

    list_destroy(list1);
    list_destroy(list2);
}

void test_clear() {
    struct list *list = list_create();

    char *item1 = "test1";
    char *item2 = "test2";
    list_push(list, item1);
    list_push(list, item2);

    list_clear(list);
    assert(list->head == NULL, "head pointer is null");
    assert(list->tail == NULL, "tail pointer is null");
    assert(list->length == 0, "list is empty");

    list_clear(list);
    assert(list->head == NULL, "head pointer is still null");
    assert(list->tail == NULL, "tail pointer is still null");
    assert(list->length == 0, "list is still empty");

    list_destroy(list);
}

void test_iterator() {
    struct list *list = list_create();

    char *item1 = "test1";
    char *item2 = "test2";
    list_push(list, item1);
    list_push(list, item2);

    struct iterator *nodes = list_iterator(list);
    assert(nodes->destroy != NULL, "has a destructor function");
    assert(nodes->current == NULL, "starts with null item");
    assert(nodes->index == 0, "starts at index 0");

    void *first = nodes->next(nodes);
    assert(first == item1, "returns first item");
    assert(nodes->current == item1, "stores current item");
    assert(nodes->index == 1, "increments index");

    void *second = nodes->next(nodes);
    assert(second == item2, "returns second item");
    assert(nodes->current == item2, "stores current item");
    assert(nodes->index == 2, "increments index");

    void *third = nodes->next(nodes);
    assert(third == NULL, "returns null when iteration is complete");
    assert(nodes->current == NULL, "current item is null");
    assert(nodes->index == 2, "does not increment index");

    nodes->destroy(nodes);
    list_destroy(list);
}

int main() {
    test_create();
    test_push();
    test_pop();
    test_unshift();
    test_shift();
    test_clear();
    test_clone();
    test_concat();
    test_iterator();

    return 0;
}
