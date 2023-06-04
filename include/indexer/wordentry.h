#ifndef __INDEXER_WORDENTRY_H
#define __INDEXER_WORDENTRY_H

#include "structures/list.h"
#include "fileentry.h"

struct _WordEntry {
	char	*word;
	int		total;
	List	fentries;
};

typedef struct _WordEntry WordEntry;

WordEntry	wentry_init(char *word, char *sentence, int line, char *filename);
void		wentry_destroy(WordEntry *wentry);
void		wentry_update(WordEntry *wentry, char *sentencebuf, int line, char *filename);

#endif /* __INDEXER_WORDENTRY_H */