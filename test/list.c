#include <stdlib.h>
#include <stdio.h>

#include "list.h"

int main(int argc char *argv[]) {
    struct list *list = list_create();
    if (!list) {
        exit(1);
    }

    list_destroy(list);

    return 0;
}

