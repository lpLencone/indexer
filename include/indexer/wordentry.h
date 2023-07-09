#ifndef INDEXER_WORDENTRY_H_
#define INDEXER_WORDENTRY_H_

#include "structures/list.h"
#include "fileentry.h"

struct _WordEntry {
    char   *word;
    int     total;
    List    fentries;
};

typedef struct _WordEntry WordEntry;

WordEntry   wentry_init(char *word, char *sentence, int line, char *filename);
void        wentry_destroy(WordEntry *wentry);

void        wentry_update(WordEntry *wentry, char *sentencebuf, int line, char *filename);

#endif /* INDEXER_WORDENTRY_H_ */