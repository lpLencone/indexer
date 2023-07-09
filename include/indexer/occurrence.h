#ifndef __INDEXER_OCCURRENCE_H
#define __INDEXER_OCCURRENCE_H

struct _Occurrence {
	int     line;
	char   *sentence;
};

typedef struct _Occurrence Occurrence;

Occurrence  occurrence_init(char *sentence, int sentenceline);
void        occurrence_destroy(Occurrence *occurrence);

#endif /* __INDEXER_OCCURRENCE_H */