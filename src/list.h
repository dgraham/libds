#ifndef LIST_H
#define LIST_H

struct list {
    void **nodes;
};

struct list *list_create();

void list_destroy(struct list *this);

#endif

