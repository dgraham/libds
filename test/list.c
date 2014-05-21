#include <stdlib.h>
#include <stdio.h>

#include "list.h"

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

    list_destroy(list);
}

int main(int argc, char *argv[]) {
    struct list *list = list_create();
    if (!list) {
        exit(1);
    }

    test_create();
    test_push();

    list_destroy(list);
    return 0;
}

