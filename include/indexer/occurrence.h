#ifndef INDEXER_OCCURRENCE_H_
#define INDEXER_OCCURRENCE_H_

struct _Occurrence {
    int     line;
    char   *sentence;
};

typedef struct _Occurrence Occurrence;

Occurrence  occurrence_init(char *sentence, int sentenceline);
void        occurrence_destroy(Occurrence *occurrence);

#endif /* INDEXER_OCCURRENCE_H_ */