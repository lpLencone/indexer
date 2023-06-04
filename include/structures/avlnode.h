#ifndef __STRUCTURES_AVLNODE_H
#define __STRUCTURES_AVLNODE_H

#include <stddef.h>

struct _AvlNode {
	struct _AvlNode	*left;
	struct _AvlNode	*right;
	
	int		height;
	void	*data;
};

typedef struct _AvlNode AvlNode;

AvlNode	*avlnode_init(void *data, size_t bytes);
void	avlnode_destroy(AvlNode *avlnode);



#endif /* __STRUCTURES_AVLNODE_H */