#ifndef __STRUCTURES_NODE_H
#define __STRUCTURES_NODE_H

#include <stddef.h>

struct _Node {
    struct _Node *next;
	struct _Node *prev;	

	void *data;
};

typedef struct _Node Node;

Node	*node_init(void *data, size_t bytes);
void	node_destroy(Node *node);

#endif /* __STRUCTURES_NODE_H */

