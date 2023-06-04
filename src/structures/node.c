#include "structures/node.h"

/*	NULL, malloc, free */
#include <stdlib.h>
/*	memcpy */
#include <string.h>

Node *node_init(void *data, size_t bytes)
{
	Node *node = (Node *)malloc(sizeof(Node));
	node->prev = NULL;
	node->next = NULL;

	node->data = malloc(bytes);
	memcpy(node->data, data, bytes);

	return node;
}

void node_destroy(Node *node)
{
    free(node->data);
	free(node);
}
