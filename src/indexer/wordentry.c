#include "indexer/wordentry.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int     compare_fentry(void *info, void *list_data);
static void    dealloc_fentry(void *data);

#define SIZEOFSTRING(str) (strlen(str) + 1)

WordEntry wentry_init(char *word, char *sentence, int line, char *filename)
{
    WordEntry wentry;

    wentry.word = malloc(SIZEOFSTRING(word));
    strcpy(wentry.word, word);

    /* If a word entry is being created, it is assumed that the word was found
        for the first time in a file. Hence total (frequency) = 1 */
    wentry.total = 1;

    wentry.fentries = list_init(dealloc_fentry, compare_fentry);
    FileEntry fentry = fentry_init(sentence, line, filename);

    list_append(&wentry.fentries, &fentry, sizeof(fentry));

    return wentry;
}

void wentry_destroy(WordEntry *wentry)
{
    free(wentry->word);
    list_destroy(&wentry->fentries);
}

void wentry_update(WordEntry *wentry, char *sentence, int sentenceline, char *filename)
{
    wentry->total++;

    /* Look for the file that has the word in the file list */
    FileEntry *fentry = list_search(&wentry->fentries, filename);

    if (fentry != NULL) {
        fentry_update(fentry, sentence, sentenceline);
    }
    else {
        /* Create new file entry if there's no entry with the filename */
        FileEntry new_fentry = fentry_init(sentence, sentenceline, filename);
        list_append(&wentry->fentries, &new_fentry, sizeof(new_fentry));
    }
}

static void dealloc_fentry(void *data)
{
    fentry_destroy(data);
}

static int compare_fentry(void *info, void *list_data)
{
    char *filename = info;
    FileEntry *fentry = list_data;
    return strcmp(filename, fentry->filename);
}

#undef SIZEOFSTRING

