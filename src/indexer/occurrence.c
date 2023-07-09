#include "indexer/occurrence.h"

#include <stdlib.h>
#include <string.h>

#define SIZEOFSTRING(str) (sizeof(char) * (strlen(str) + 1))

Occurrence occurrence_init(char *sentence, int sentenceline)
{
	Occurrence occur;
	occur.line = sentenceline;
	occur.sentence = malloc(SIZEOFSTRING(sentence));
	strcpy(occur.sentence, sentence);
	return occur;
}

void occurrence_destroy(Occurrence *occur)
{
    free(occur->sentence);
}

#undef SIZEOFSTRING
