#include "structures/avltree.h"

#include <stdio.h>
#include <string.h>

#define func_compare	(*compare)(void *data, void *tree_data)
#define func_dealloc	(*dealloc)(void *data)

/* Usa função de liberar `dealloc` atribuido à árvore no momento de sua inicialização 
	para liberar os dados possivelmente dinamicamente alocados aos quais o membro 
	`data` do nó aponta. */
static void		avltree_destroy_recursive(AvlNode *avlnode, void func_dealloc);

/* Se o nó não for nulo, retorna sua altura; se for nulo, retorna 0 */
static int		height(AvlNode *node);

/* Utiliza a função `height` para obter as alturas dos nós filhos e retorna
	a diferença entre elas; se `node` for nulo, retorna 0 */
static int		get_balance(AvlNode *node); /* Balanço */

/* Função que retorna o maior valor entre dois números inteiros;
	utilizada, neste programa, para determinar a altura de um nó a partir
	da altura de seus filhos. */
static int		max(int a, int b);

static AvlNode	*rotate_left(AvlNode *node);
static AvlNode	*rotate_right(AvlNode *node);
static AvlNode	*avltree_insert_recursive(AvlTree *tree, AvlNode *newnode, AvlNode *treenode);

static AvlNode	*avltree_search_recursive(AvlTree *tree, AvlNode *cursor, void *info, int *direction);

AvlTree avltree_init(void func_dealloc, int func_compare)
{
	AvlTree tree;

	tree.root    = NULL;
	tree.dealloc = dealloc;
	tree.compare = compare;

	return tree;
}

void avltree_destroy(AvlTree *tree)
{
	avltree_destroy_recursive(tree->root, tree->dealloc);
}

void avltree_insert(AvlTree *tree, void *data, size_t bytes)
{
	AvlNode *newnode = avlnode_init(data, bytes);
	tree->root = avltree_insert_recursive(tree, newnode, tree->root);
}

void *avltree_search(AvlTree *tree, void *info)
{
	if (tree->root != NULL) {
		int direction;
		AvlNode *cursor = avltree_search_recursive(tree, tree->root, info, &direction);
		if (direction == 0) {
			return cursor->data;
		}
	}
    return NULL;
}

/*********************
 * PRIVATE FUNCTIONS *
 *********************/

static void avltree_destroy_recursive(AvlNode *node, void func_dealloc)
{
	if (node != NULL) {
		avltree_destroy_recursive(node->left, dealloc);
		avltree_destroy_recursive(node->right, dealloc);
		if (dealloc != NULL) {
			dealloc(node->data);
		}
		avlnode_destroy(node);
	}
}

static AvlNode *avltree_search_recursive(AvlTree *tree, AvlNode *cursor, void *info, int *direction)
{
    // Comparar `info` fornecida e o dado do cursor dessa iteração na árvore
    *direction = tree->compare(info, cursor->data);

    // dado é menor que cursor data
    if (*direction < 0) {
        // Se o cursor tiver um filho com um valor menor (é apontado por `left`)...
        if (cursor->left != NULL) {
            // Torne-o o cursor da próxima iteração e retorne a direção
            return avltree_search_recursive(tree, cursor->left, info, direction);
        }
        else {
            // Retorne o cursor e defina a direção para -1, pois a `info` é menor que o cursor->data
            return cursor;
        }
    }
    // Dado é maior que cursor->data
    if (*direction > 0) {
        // Se o cursor tiver um filho com um valor maior...
        if (cursor->right != NULL) {
            // Torne-o o cursor da próxima iteração
            return avltree_search_recursive(tree, cursor->right, info, direction);
        }
        else {
            // Retorne o cursor e defina a direção para 1, pois a `info` é maior que o cursor->data
            return cursor;
        }
    }

    // A `info` é igual ao dado do cursor, o que significa que o dado foi encontrado
    *direction = 0;
    return cursor;
}

static AvlNode *avltree_insert_recursive(AvlTree *tree, AvlNode *newnode, AvlNode *treenode)
{
	if (treenode == NULL) {
		return newnode;
	}

	int func_compare = tree->compare;

	int resultado = compare(newnode->data, treenode->data);

	/* Se o dado do newnode for menor que o dado do treenode */
	if (resultado < 0) {
		/* Inserir recursivamente pelo nó à esquerda do treenode */
		treenode->left = avltree_insert_recursive(tree, newnode, treenode->left);
	}
	/* Se o dado do `newnode` for maior que o dado do `treenode` */
	else if (resultado > 0) {
		/* Inserir recursivamente pelo nó à direta do treenode */
		treenode->right = avltree_insert_recursive(tree, newnode, treenode->right);
	}
	/* Se os dados forem iguais */
	else {
		/* Destruir o `newnode`, pois árvores de pesquisa binária não
			aceitam nós duplicados. */
		tree->dealloc(newnode->data);
		avlnode_destroy(newnode);
		/* Retornar treenode como raíz da subárvore a partir da qual
			a função foi chamada */
		return treenode;
	}

	/* Definir a altura do treenode como a maior altura entre seus
		nós filhos somado a 1 */
	treenode->height = 1 + max(height(treenode->left), height(treenode->right));

	/* Obter balanço para verificar se é necessário balancear a árvore
		e como rotacionar para atingir o balanço */
	int balance = get_balance(treenode);

	/* Se o balanço pesar para a esquerda */
	if (balance > 1) {
		/* Comparar o data do filho à esquerda e o newnode nó */
		int comparison = compare(newnode->data, treenode->left->data);

		/* Se o newnode for maior que treenode->left */
		if (comparison > 0) {
			/* Left Right */
			treenode->left = rotate_left(treenode->left);
			return rotate_right(treenode);
		}

		/* Se o newnode for menor que treenode->left */
		else if (comparison < 0) {
			/* Left Left */
			return rotate_right(treenode);
		}

	}
	/* Se o balanço pesar para a direita */
	else if (balance < -1) {
		/* Comparar o data do filho à direita e o newnode nó */
		int comparison = compare(newnode->data, treenode->right->data);

		/* Se o newnode for maior que treenode->right */
		if (comparison > 0) {
			// Right Right
			return rotate_left(treenode);
		}

		/* Se o newnode for menor que treenode->right */
		else if (comparison < 0) {
			// Right Left
			treenode->right = rotate_right(treenode->right);
			return rotate_left(treenode);
		}
	}

	return treenode;
}

static int max(int a, int b)
{
	return (a > b) ? a : b;
}

static int height(AvlNode *node)
{
	if (node == NULL) {
		return 0;
	}
	return node->height;
}

static int get_balance(AvlNode *node)
{
	if (node == NULL) {
		return 0;
	}
	return height(node->left) - height(node->right);
}

static AvlNode *rotate_left(AvlNode *x)
{
	AvlNode *y = x->right;
	AvlNode *T2 = y->left;

	y->left = x;
	x->right = T2;

	x->height = 1 + max(height(x->left), height(x->right));
	y->height = 1 + max(height(y->left), height(y->right));

	return y;
}

static AvlNode *rotate_right(AvlNode *y)
{
	AvlNode *x = y->left;
	AvlNode *T2 = x->right;

	x->right = y;
	y->left = T2;

	y->height = 1 + max(height(y->left), height(y->right));
	x->height = 1 + max(height(x->left), height(x->right));

	return x;
}

#undef func_compare
#undef func_dealloc
