#include "structures/list.h"

#include <stdlib.h>
#include <string.h>

#define func_dealloc	(*dealloc)(void *data)
#define func_compare	(*compare)(void *info, void *list_data)

List list_init(void func_dealloc, int func_compare)
{
	List list;

	list.dealloc = dealloc;
	list.compare = compare;

	list.first	= NULL;
	list.last	= NULL;
	list.length	= 0;

	return list;
}

void list_destroy(List *list)
{
	Node *cursor = list->first;
    while (cursor != NULL) {
        Node *aux = cursor->next;

		if (list->dealloc != NULL) {
			list->dealloc(cursor->data);
		}
        node_destroy(cursor);

        cursor = aux;
    }
}

void list_append(List *list, void *data, size_t bytes)
{
	Node *node = node_init(data, bytes);

	if (list->length == 0) {
		list->first = node;
	}
	else {
		node->prev = list->last;
		list->last->next = node;
	}
	list->last = node;
	list->length++;
}

void *list_search(List *list, void *info)
{
	Node *cursor = list->first;
	while (cursor != NULL) {
		if (list->compare(info, cursor->data) == 0) {
			return cursor->data;
		}
		cursor = cursor->next;
	}
	return NULL;
}

#undef func_compare
#undef func_dealloc

