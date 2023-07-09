#include "structures/avltree.h"

#include <stdio.h>
#include <string.h>

/* Use the dealloc function assiged to it on its initialization to free the data
    potentially dinamically allocated which the `avlnode->data` member points to. */
static void avltree_destroy_recursive(AvlNode *avlnode, DeallocFunc dealloc);

/* If node isn't NULL, return its height; else 0 */
static int  height(AvlNode *node);

/* Use the `height()` function to get each child node's height and return
    the difference between these heights; if `node` is NULL, return 0 */
static int  get_balance(AvlNode *node); /* Balanço */

/* Yo classic max func */
static int  max(int a, int b);

static AvlNode   *rotate_left(AvlNode *node);
static AvlNode   *rotate_right(AvlNode *node);
static AvlNode   *avltree_insert_recursive(AvlTree *tree, AvlNode *newnode, AvlNode *treenode);

static AvlNode   *avltree_search_recursive(AvlTree *tree, AvlNode *cursor, void *info, int *direction);

AvlTree avltree_init(DeallocFunc dealloc, CompareFunc compare)
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

static void avltree_destroy_recursive(AvlNode *node, DeallocFunc dealloc)
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
    *direction = tree->compare(info, cursor->data);

    if (*direction < 0) {
        if (cursor->left != NULL) {
            return avltree_search_recursive(tree, cursor->left, info, direction);
        }
        else {
            return cursor;
        }
    }
    if (*direction > 0) {
        if (cursor->right != NULL) {
            return avltree_search_recursive(tree, cursor->right, info, direction);
        }
        else {
            return cursor;
        }
    }

    *direction = 0;
    return cursor;
}

static AvlNode *avltree_insert_recursive(AvlTree *tree, AvlNode *newnode, AvlNode *treenode)
{
    if (treenode == NULL) {
        return newnode;
    }

    CompareFunc compare = tree->compare;

    int resultado = compare(newnode->data, treenode->data);

    if (resultado < 0) {
        treenode->left = avltree_insert_recursive(tree, newnode, treenode->left);
    }
    else if (resultado > 0) {
        treenode->right = avltree_insert_recursive(tree, newnode, treenode->right);
    }
    else {
        tree->dealloc(newnode->data);
        avlnode_destroy(newnode);
        return treenode;
    }

    treenode->height = 1 + max(height(treenode->left), height(treenode->right));

    int balance = get_balance(treenode);

    if (balance > 1) {
        int comparison = compare(newnode->data, treenode->left->data);

        if (comparison > 0) {
            treenode->left = rotate_left(treenode->left);
            return rotate_right(treenode);
        }

        else if (comparison < 0) {
            return rotate_right(treenode);
        }

    }
    else if (balance < -1) {
        int comparison = compare(newnode->data, treenode->right->data);

        if (comparison > 0) {
            return rotate_left(treenode);
        }
        else if (comparison < 0) {
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

