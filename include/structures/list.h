#ifndef __STRUCTURES_LIST_H
#define __STRUCTURES_LIST_H

#define func_dealloc	(*dealloc)(void *data)
#define func_compare	(*compare)(void *info, void *list_data)

#include "node.h"

struct _List {
	Node   *first;
	Node   *last;
	int     length;

	void    func_dealloc;
	int     func_compare;
};

typedef struct _List List;

List	list_init(void func_dealloc, int func_compare);
void	list_destroy(List *list);
void	list_append(List *list, void *data, size_t bytes);

/* Procura na lista um nó a partir da `info` fornecida para a pesquisa, 
	chamando internamente a função de comparação atribuida durante a 
	inicialização da lista. Se o nó for encontrado, seu dado será retornado; 
	se não, retorna NULL. */
/* Search in the list a node using the provided `info` by calling the
	compare function assigned to the list on its creation. If the node is
	found, its data will be returned; else, NULL. */
void  *list_search(List *list, void *info);

#undef func_compare
#undef func_dealloc

#endif /* __STRUCTURES_LIST_H */