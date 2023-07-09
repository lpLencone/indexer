#ifndef STRUCTURES_NODE_H_
#define STRUCTURES_NODE_H_

#include <stddef.h>

struct _Node {
    struct _Node *next;
    void         *data;
};

typedef struct _Node Node;

Node   *node_init(void *data, size_t bytes);
void    node_destroy(Node *node);

#endif /* STRUCTURES_NODE_H_ */

