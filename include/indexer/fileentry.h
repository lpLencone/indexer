#ifndef __INDEXER_FILEENTRY_H
#define __INDEXER_FILEENTRY_H

#include "structures/list.h"
#include "occurrence.h"

struct _FileEntry {
	char   *filename;
	int		frequency;
	int		n_lines;
	List	occurrences;
};

typedef struct _FileEntry FileEntry;

FileEntry	fentry_init(char *sentence, int line, char *filename);
void		fentry_destroy(FileEntry *fentry);
void		fentry_update(FileEntry *fentry, char *sentence, int line);

#endif /* __INDEXER_FILEENTRY_H */