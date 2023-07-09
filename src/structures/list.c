#include "structures/list.h"

#include <stdlib.h>
#include <string.h>

typedef void (*DeallocFunc)(void *data);
typedef int  (*CompareFunc)(void *info, void *list_data);

List list_init(DeallocFunc dealloc, CompareFunc compare)
{
    List list;

    list.root    = NULL;
    list.dealloc = dealloc;
    list.compare = compare;

    return list;
}

void list_destroy(List *list)
{
    Node *cursor = list->root;
    while (cursor != NULL) {
        Node *aux = cursor->next;

        if (list->dealloc != NULL) {
            list->dealloc(cursor->data);
        }
        node_destroy(cursor);

        cursor = aux;
    }
}

void list_prepend(List *list, void *data, size_t bytes)
{
    Node *node = node_init(data, bytes);

    if (list->root == NULL) {
        list->root = node;
    }
    else {
        node->next = list->root;
        list->root = node;
    }
}

void *list_search(List *list, void *info)
{
    Node *cursor = list->root;
    while (cursor != NULL) {
        if (list->compare(info, cursor->data) == 0) {
            return cursor->data;
        }
        cursor = cursor->next;
    }
    return NULL;
}


