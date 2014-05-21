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

    assert(list->first == NULL, "first pointer is null");
    assert(list->last == NULL, "last pointer is null");
    assert(list->length == 0, "list is empty");

    list_destroy(list);
}

void test_push() {
    struct list *list = list_create();

    char *item1 = "test1";
    assert(list_push(list, item1), "first item accepted");
    assert(list->first != NULL, "first pointer is set");
    assert(list->last != NULL, "last pointer is set");
    assert(list->last == list->first, "last pointer equals first");
    assert(list->length == 1, "length is incremented");
    assert(list->first->value == item1, "stored item value");
    assert(list->first->next == NULL, "first item has no next item");

    char *item2 = "test2";
    assert(list_push(list, item2), "second item accepted");
    assert(list->first != NULL, "first pointer is set");
    assert(list->last != NULL, "last pointer is set");
    assert(list->last != list->first, "moved last pointer");
    assert(list->length == 2, "length is incremented");
    assert(list->last->value == item2, "stored item value");
    assert(list->first->next == list->last, "first item points to last item");

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

