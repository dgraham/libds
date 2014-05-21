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

int main(int argc, char *argv[]) {
    struct list *list = list_create();
    if (!list) {
        exit(1);
    }

    test_create();

    list_destroy(list);
    return 0;
}

