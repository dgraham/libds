# Simple Data Structures in C

Because, after forty short years, a hash table isn't in the standard library yet.

## Hash table

Stores key/value pairs in insertion order.

```c
// allocate memory
struct hashmap *map = hashmap_create();

// store first key/value pair
int id = 42;
struct hkey key = {&id, sizeof(id)};
char *a = "item 1";

hashmap_get(map, &key);      // => NULL
hashmap_set(map, &key, a);   // => NULL
hashmap_get(map, &key);      // => "item 1"
hashmap_contains(map, &key); // => true

// store another key
int id2 = 12;
struct hkey key2 = {&id2, sizeof(id2)};
char *b = "item 2";

hashmap_set(map, &key2, b);  // => NULL

// iterate through keys in insertion order
struct iterator *entries = hashmap_iterator(map);
while (entries->next(entries)) {
    struct hentry *entry = entries->current;
    printf("%d => %s\n", entry->key, entry->value);
}

// free memory
entries->destroy(entries);
hashmap_destroy(map);
```

## Heap

Store nodes in sorted order. Useful as a priority queue.

```c
// define comparator function
int compare_nodes(const void *a, const void *b) {
    return strcmp(a, b);
}

// allocate memory
struct heap *queue = heap_create(compare_nodes);

// add nodes
char *a = "item 1";
char *b = "item 2";
heap_push(queue, b);
heap_push(queue, a);

// remove nodes in sorted order
heap_pop(queue); // => "item 1"
heap_pop(queue); // => "item 2"
heap_pop(queue); // => NULL

// free memory
heap_destroy(queue);
```

## Linked List

Dynamically sized list. Useful as a queue.

```c
// allocate memory
struct list *queue = list_create();

// append items to queue
char *a = "item 1";
char *b = "item 2";
list_push(queue, a);
list_push(queue, b);

// iterate through items
struct iterator *items = list_iterator(queue);
while (items->next(items)) {
    char *item = items->current;
    printf("[%d] => %s\n", items->index, item);
}

// remove from front of queue
list_shift(queue); // => "item 1"
list_shift(queue); // => "item 2"
list_shift(queue); // => NULL

// free memory
items->destroy(items);
list_destroy(queue);
```

## Vector

Dynamically sized array. Useful as a stack.

```c
// allocate memory
struct vector *stack = vector_create();

// push items onto the stack
char *a = "item 1";
char *b = "item 2";
vector_push(stack, a);
vector_push(stack, b);

// iterate through items
struct iterator *items = vector_iterator(stack);
while (items->next(items)) {
    char *item = items->current;
    printf("[%d] => %s\n", items->index, item);
}

// pop items off
vector_pop(vector); // => "item 2"
vector_pop(vector); // => "item 1"

// free memory
items->destroy(items);
vector_destroy(stack);
```
