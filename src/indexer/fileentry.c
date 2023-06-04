#include "indexer/fileentry.h"

#include <stdlib.h>
#include <string.h>

/* Função que a lista de ocorrências da estrutura FileEntry deverá chamar
	para comparar ocorrências. */
static int		compare_occurrence(void *info, void *list_data);

/* Função que a lista de ocorrências da estrutura FileEntry deverá chamar
	para desalocar a memória alocada para o membro `sentence` da estrutura
	Occurrence, à qual a lista não consegue referenciar diretamente. */
static void		dealloc_occurrence(void *data);

#define SIZEOFSTRING(str) (sizeof(char) * (strlen(str) + 1))

FileEntry fentry_init(char *sentence, int line, char *filename)
{
    FileEntry fentry;
	
	fentry.filename = (char *)malloc(SIZEOFSTRING(filename));
	strcpy(fentry.filename, filename);

	/* Assume-se que, ao criar um registro de arquivo, é porque foi encontrado
		uma palavra presente naquele arquivo pela primeira vez, por isso sua
		frequência é inicializada como 1. */
	fentry.frequency = 1;
	fentry.n_lines = line;

	fentry.occurrences = list_init(dealloc_occurrence, compare_occurrence);

	Occurrence occur = occurrence_init(sentence, line);
	list_append(&fentry.occurrences, (void *)&occur, sizeof(occur));

	return fentry;
}

void fentry_destroy(FileEntry *fentry)
{
	free(fentry->filename);
	list_destroy(&fentry->occurrences);
}

void fentry_update(FileEntry *fentry, char *sentence, int line)
{
	fentry->frequency++;
	fentry->n_lines = line;

	/* Verifique se a frase `sentence` já está na lista de ocorrências
		e se não estiver, adicione-a. */
	Occurrence *occur = (Occurrence *)list_search(&fentry->occurrences, sentence);
	if (occur == NULL) {
		Occurrence new_occur = occurrence_init(sentence, line);
		list_append(&fentry->occurrences, (void *)&new_occur, sizeof(new_occur));
	}
}

static int compare_occurrence(void *info, void *list_data)
{
	Occurrence *occur = (Occurrence *)list_data;
	return strcmp((char *)info, occur->sentence);
}

static void dealloc_occurrence(void *data)
{
	occurrence_destroy((Occurrence *)data);
}

#undef SIZEOFSTRING
