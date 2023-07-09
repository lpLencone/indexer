#include "indexer/fileentry.h"

#include <stdlib.h>
#include <string.h>

/* Function that the occurrences list member of the FileEntry struct will
    call to compare the occurrences */
static int  compare_occurrence(void *info, void *list_data);

/* Function that the occurrences list member of the FileEntry struct will
    call to deallocate the memory used by the `sentence` member of the
    Occurrence struct. */
static void dealloc_occurrence(void *data);

#define SIZEOFSTRING(str) (strlen(str) + 1)

FileEntry fentry_init(char *sentence, int line, char *filename)
{
    FileEntry fentry;
    
    fentry.filename = malloc(SIZEOFSTRING(filename));
    strcpy(fentry.filename, filename);

    /* It is assumed that, when a file entry is created, it is because a word
        from its content was found for the first time, hence frequency = 1 */
    fentry.frequency = 1;
    fentry.n_lines = line;

    fentry.occurrences = list_init(dealloc_occurrence, compare_occurrence);

    Occurrence occur = occurrence_init(sentence, line);
    list_prepend(&fentry.occurrences, &occur, sizeof(occur));

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

    /* Check whether the `sentence` already is in the occurrences list
        if not, add it. */
    Occurrence *occur = (Occurrence *)list_search(&fentry->occurrences, sentence);
    if (occur == NULL) {
        Occurrence new_occur = occurrence_init(sentence, line);
        list_prepend(&fentry->occurrences, (void *)&new_occur, sizeof(new_occur));
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
