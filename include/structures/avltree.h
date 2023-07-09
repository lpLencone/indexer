#ifndef STRUCTURES_AVLTREE_H_
#define STRUCTURES_AVLTREE_H_

#include "avlnode.h"

typedef void (*DeallocFunc)(void *data);
typedef int	 (*CompareFunc)(void *info, void *tree_data);

struct _AvlTree {
    AvlNode     *root;
    /* User-defined function used to free the memory of the allocated
        data inserted in the tree */
    DeallocFunc  dealloc;
    /* User-defined function used to compare data from two tree nodes */
    CompareFunc  compare;
};

typedef struct _AvlTree AvlTree;

AvlTree avltree_init(DeallocFunc dealloc, CompareFunc compare);
void    avltree_destroy(AvlTree *tree);

void    avltree_insert(AvlTree *tree, void *data, size_t bytes);
void   *avltree_search(AvlTree *tree, void *info);

#endif /* STRUCTURES_AVLTREE_H_ */