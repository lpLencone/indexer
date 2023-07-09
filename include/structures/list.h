#ifndef STRUCTURES_LIST_H_
#define STRUCTURES_LIST_H_

typedef void    (*DeallocFunc)(void *data);
typedef int     (*CompareFunc)(void *info, void *list_data);

#include "node.h"

struct _List {
    Node   *first;
    Node   *last;
    int     length;

    DeallocFunc dealloc;
    CompareFunc compare;
};

typedef struct _List List;

List    list_init(DeallocFunc dealloc, CompareFunc compare);
void    list_destroy(List *list);
void    list_append(List *list, void *data, size_t bytes);

/* Search in the list a node using the provided `info` by calling the
    compare function assigned to the list on its creation. If the node is
    found, its data will be returned; else, NULL. */
void  *list_search(List *list, void *info);

#endif /* STRUCTURES_LIST_H_ */