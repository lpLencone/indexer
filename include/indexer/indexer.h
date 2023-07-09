#ifndef __INDEXER_H
#define __INDEXER_H	

#include "wordentry.h"
#include "structures/avltree.h"

struct _Indexer {
    int     n_files;
    char  **filenames;

    AvlTree wentries;
};

typedef struct _Indexer Indexer;

Indexer	indexer_init(char **filenames, int n_files);
void    indexer_destroy(Indexer *indexer);

void indexer_print(Indexer *indexer, char *words, int n_occurrences_file);

#endif /* __INDEXER_H */

