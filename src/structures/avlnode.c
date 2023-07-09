#include "structures/avlnode.h"

#include <stdlib.h>
#include <string.h>

AvlNode *avlnode_init(void *data, size_t bytes)
{
    AvlNode *node = malloc(sizeof(AvlNode));

    node->height = 1;
    node->left   = NULL;
    node->right  = NULL;

    node->data = malloc(bytes);
    memcpy(node->data, data, bytes);

    return node;
}

void avlnode_destroy(AvlNode *node)
{
    free(node->data);
    free(node);
}
