#ifndef __STRUCTURES_AVLTREE_H
#define __STRUCTURES_AVLTREE_H

#include "avlnode.h"

#define func_dealloc (*dealloc)(void *data)
#define func_compare (*compare)(void *info, void *tree_data)

struct _AvlTree {
	AvlNode	*root;

	/* Função definida pelo usuário da API que será usada para liberar
		memória dos dados alocados inseridos na árvore */
	/* User-defined function used to free the memory of the allocated
		data inserted in the tree */
	void    func_dealloc;

	/* Função definida pelo usuário da API que será usada para comparar
		dados entre os nós da árvore */
	/* User-defined function used to compare data from two tree nodes */
	int	    func_compare;
};

typedef struct _AvlTree AvlTree;

AvlTree avltree_init(void func_dealloc, int func_compare);
void    avltree_destroy(AvlTree *tree);

void    avltree_insert(AvlTree *tree, void *data, size_t bytes);
void   *avltree_search(AvlTree *tree, void *info);

#undef func_dealloc
#undef func_compare

#endif /* __STRUCTURES_AVLTREE_H */