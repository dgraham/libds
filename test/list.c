#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "list.h"

void test_create(void);
void test_push(void);
void test_pop(void);
void test_unshift(void);
void test_shift(void);
void test_clone(void);
void test_concat(void);
void test_clear(void);
void test_iterator(void);

void test_create() {
    struct list *list = list_create();

    assert(list->head == NULL);
    assert(list->tail == NULL);
    assert(list->length == 0);

    list_destroy(list);
}

void test_push() {
    struct list *list = list_create();

    char *item1 = "test1";
    assert(list_push(list, item1));
    assert(list->head != NULL);
    assert(list->tail != NULL);
    assert(list->tail == list->head);
    assert(list->length == 1);
    assert(list->head->value == item1);
    assert(list->head->prev == NULL);
    assert(list->head->next == NULL);

    char *item2 = "test2";
    assert(list_push(list, item2));
    assert(list->head != NULL);
    assert(list->tail != NULL);
    assert(list->tail != list->head);
    assert(list->length == 2);
    assert(list->tail->value == item2);
    assert(list->head->next == list->tail);
    assert(list->tail->next == NULL);
    assert(list->tail->prev == list->head);
    assert(list->head->prev == NULL);

    list_destroy(list);
}

void test_pop() {
    struct list *list = list_create();

    char *item1 = "test1";
    char *item2 = "test2";
    list_push(list, item1);
    list_push(list, item2);

    assert(list_pop(list) == item2);
    assert(list->head != NULL);
    assert(list->tail != NULL);
    assert(list->tail == list->head);
    assert(list->length == 1);
    assert(list->head->value == item1);
    assert(list->head->prev == NULL);
    assert(list->head->next == NULL);

    assert(list_pop(list) == item1);
    assert(list->head == NULL);
    assert(list->tail == NULL);
    assert(list->length == 0);

    assert(list_pop(list) == NULL);
    assert(list->length == 0);

    list_destroy(list);
}

void test_unshift() {
    struct list *list = list_create();

    char *item1 = "test1";
    assert(list_unshift(list, item1));
    assert(list->head != NULL);
    assert(list->tail != NULL);
    assert(list->tail == list->head);
    assert(list->length == 1);
    assert(list->head->value == item1);
    assert(list->head->prev == NULL);
    assert(list->head->next == NULL);

    char *item2 = "test2";
    assert(list_unshift(list, item2));
    assert(list->head != NULL);
    assert(list->tail != NULL);
    assert(list->tail != list->head);
    assert(list->length == 2);
    assert(list->tail->value == item1);
    assert(list->head->value == item2);
    assert(list->head->next == list->tail);
    assert(list->tail->next == NULL);
    assert(list->tail->prev == list->head);
    assert(list->head->prev == NULL);

    list_destroy(list);
}

void test_shift() {
    struct list *list = list_create();

    char *item1 = "test1";
    char *item2 = "test2";
    list_push(list, item1);
    list_push(list, item2);

    assert(list_shift(list) == item1);
    assert(list->head != NULL);
    assert(list->tail != NULL);
    assert(list->tail == list->head);
    assert(list->length == 1);
    assert(list->head->value == item2);
    assert(list->head->prev == NULL);
    assert(list->head->next == NULL);

    assert(list_shift(list) == item2);
    assert(list->head == NULL);
    assert(list->tail == NULL);
    assert(list->length == 0);

    assert(list_shift(list) == NULL);
    assert(list->length == 0);

    list_destroy(list);
}

void test_clone() {
    struct list *list = list_create();

    char *item1 = "test1";
    char *item2 = "test2";
    list_push(list, item1);
    list_push(list, item2);

    struct list *clone = list_clone(list);
    assert(clone != NULL);
    assert(clone != list);

    assert(clone->length == 2);
    assert(clone->head != NULL);
    assert(clone->tail != NULL);
    assert(clone->head->value == item1);
    assert(clone->tail->value == item2);
    assert(clone->head->prev == NULL);
    assert(clone->head->next == clone->tail);

    assert(clone->head != list->head);
    assert(clone->tail != list->tail);

    assert(list_pop(clone) == item2);
    assert(clone->length == 1);
    assert(list->length == 2);

    list_destroy(list);
    list_destroy(clone);
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

    assert(list_concat(list1, list2));
    assert(list1->length == 4);
    assert(list2->length == 2);

    list_push(list1, item4);
    assert(list2->tail->next == NULL);

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
    assert(list->head == NULL);
    assert(list->tail == NULL);
    assert(list->length == 0);

    list_clear(list);
    assert(list->head == NULL);
    assert(list->tail == NULL);
    assert(list->length == 0);

    list_destroy(list);
}

void test_iterator() {
    struct list *list = list_create();

    char *item1 = "test1";
    char *item2 = "test2";
    list_push(list, item1);
    list_push(list, item2);

    struct iterator *nodes = list_iterator(list);
    assert(nodes->destroy != NULL);
    assert(nodes->current == NULL);
    assert(nodes->index == 0);

    void *first = nodes->next(nodes);
    assert(first == item1);
    assert(nodes->current == item1);
    assert(nodes->index == 0);

    void *second = nodes->next(nodes);
    assert(second == item2);
    assert(nodes->current == item2);
    assert(nodes->index == 1);

    void *third = nodes->next(nodes);
    assert(third == NULL);
    assert(nodes->current == NULL);
    assert(nodes->index == 1);

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
