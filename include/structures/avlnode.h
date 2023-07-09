#ifndef STRUCTURES_AVLNODE_H_
#define STRUCTURES_AVLNODE_H_

#include <stddef.h>

struct _AvlNode {
    struct _AvlNode *left;
    struct _AvlNode *right;
    
    int     height;
    void   *data;
};

typedef struct _AvlNode AvlNode;

AvlNode *avlnode_init(void *data, size_t bytes);
void     avlnode_destroy(AvlNode *avlnode);



#endif /* STRUCTURES_AVLNODE_H_ */