#include "indexer/indexer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* In order to read sentences longer than 1028 characters, compile the program with
    INDEXER_SENTENCE_BUFSIZE set to a bigger value */
#ifndef INDEXER_SENTENCE_BUFSIZE
# define INDEXER_SENTENCE_BUFSIZE 1028
#endif

/* In order to read words longer than 32 characters, compile the program with
    INDEXER_WORD_BUFSIZE set to a bigger value */
#ifndef INDEXER_WORD_BUFSIZE
# define INDEXER_WORD_BUFSIZE 32
#endif

static void     wordindexer(char *filename, AvlTree *tree);

static void     updateword(AvlTree *tree, char *word, char *sentence, int line, char *filename);

/* Read a string from `file` to `sentencebuf` until the either one of '.' (period), '?' 
    question mark, or '!' (exclamation point) pops up. */
static int      getsentence(FILE *file, char *sentencebuf, int bufsize, int *line);

/* Put one word from `sentencebuf` in `wordbuf` and return a pointer to the
    next unread character of `sentencebuf` */
static char    *getword(char *sentencebuf, char *wordbuf, int bufsize);

/* Check whether `c` is a valid 'word character.' */
static int      valid_in_word(char c);

/* Count the number of digits in an integer. */
static int      count_digits(int n);

/* Remove leading whitespaces and multiple whitespaces in between words. A
    whitespace is anything the ctypes.h isspace() function says it is. */
static void     remove_redundant_spaces(char *s);

static void     dealloc_wentry(void *wentry);
static int      compare_wentry(void *info, void *tree_data);

/* Look for an entry the contains `word` in the tree; return NULL if there's none. */
static WordEntry *indexer_search(AvlTree *tree, char *word);

#define SIZEOFSTRING(str) (strlen(str) + 1)

Indexer indexer_init(char **filenames, int n_files)
{
    Indexer indexer;
    indexer.n_files = n_files;

    indexer.filenames = malloc(n_files * sizeof(char *));
    for (int i = 0; i < n_files; i++) {
        indexer.filenames[i] = malloc(SIZEOFSTRING(filenames[i]));
        strcpy(indexer.filenames[i], filenames[i]);
        printf("File added: %s\n", indexer.filenames[i]);
    }

    printf("All files were added.\n");
    printf("===================================\n\n");

    indexer.wentries = avltree_init(dealloc_wentry, compare_wentry);
    for (int i = 0; i < n_files; i++) {
        printf("Reading file: %s\n", indexer.filenames[i]);
        wordindexer(indexer.filenames[i], &indexer.wentries);
    }

    printf("All files were read.\n");
    printf("===================================\n\n");


    return indexer;
}

void indexer_destroy(Indexer *indexer)
{
    for (int i = 0; i < indexer->n_files; i++) {
        free(indexer->filenames[i]);
    }
    free(indexer->filenames);
    avltree_destroy(&indexer->wentries);
}

void indexer_print(Indexer *indexer, char *word, int n_occurrences_file)
{
    /* Search for word in tree */
    WordEntry *wentry = indexer_search(&indexer->wentries, word);

    /* If word exists */
    if (wentry != NULL) {
        /* Print its total frequency */
        printf("Total frequency: %d\n", wentry->total);

        /* Loop through the file list in which the word appears */
        Node *fentry_cursor = wentry->fentries.first;
        while (fentry_cursor != NULL) {
            FileEntry *fentry = fentry_cursor->data;
            
            /* Print the file name and its frequency in the file */
            printf("--| File: %s\n", fentry->filename);
            printf("--| Frequency: %d\n", fentry->frequency);

            /* Create a format for occurrences in that file based off its line count */
            char occurrence_format[64];
            int line_digits = count_digits(fentry->n_lines);
            int occurrence_digits = count_digits(n_occurrences_file);
            sprintf(occurrence_format, "[%%%dd]-[%%%dd] %%s\n", occurrence_digits, line_digits);
            
            int occurrence_num = 1;
            /* Loop through the word's occurrences list in the file and print the
                sentences in which it occurs */
            Node *occurrence_cursor = fentry->occurrences.first;
            while (occurrence_cursor != NULL && occurrence_num <= n_occurrences_file) {
                Occurrence *occurrence = occurrence_cursor->data;
                printf(occurrence_format, occurrence_num++, occurrence->line, occurrence->sentence);

                occurrence_cursor = occurrence_cursor->next;    
            }

            fentry_cursor = fentry_cursor->next;
            printf("\n");
        }
        printf("\n===================================\n\n");
    }
    /* If the word doesn't exist */
    else {
        printf("Word \"%s\" wasn't found in any of the files.\n", word);
        printf("\n===================================\n\n");
    }
}



/*********************
 * PRIVATE FUNCTIONS *
 *********************/

static void wordindexer(char *filename, AvlTree *tree)
{
    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        perror("Could not open file");
        exit(1);
    }

    char sentencebuf[INDEXER_SENTENCE_BUFSIZE];
    char wordbuf[INDEXER_WORD_BUFSIZE];
    
    int end_of_file;
    int line = 1;
    /* Get all sentences of a file. */
    do {
        /* Store the line at which the sentence started */
        int sentence_start_line = line;
        end_of_file = getsentence(file, sentencebuf, INDEXER_SENTENCE_BUFSIZE, &line);

        /* If a valid sentence was obtained */
        if (*sentencebuf != '\0') {
            char *sentencebuf_pos = sentencebuf;

            /* Get all words in a sentence */
            do {
                sentencebuf_pos = getword(sentencebuf_pos, wordbuf, INDEXER_WORD_BUFSIZE);

                /* If a valid word was found, update its data. */
                if (strlen(wordbuf) > 0) {
                    updateword(tree, wordbuf, sentencebuf, sentence_start_line, filename);
                }
            }
            while (*sentencebuf_pos != '\0');
        }
    }
    while (!end_of_file);

    fclose(file);
}

static void updateword(AvlTree *tree, char *word, char *sentence, int line, char *filename)
{
    /* Look for `word` in the tree*/
    WordEntry *wentry = indexer_search(tree, word);

    if (wentry != NULL) {
        wentry_update(wentry, sentence, line, filename);
    }
    else {
        /* Create new word entry if it doesn't exist yet. */
        WordEntry new_wentry = wentry_init(word, sentence, line, filename);
        avltree_insert(tree, &new_wentry, sizeof(new_wentry));
    }
}

static int getsentence(FILE *file, char *sentencebuf, int bufsize, int *line)
{
    char *sentencebuf_pos = sentencebuf;

    char c;
    int offset = 0;
    int newline = 0;

    while ((c = fgetc(file)) != EOF) {
        /* Check whether the sentence being read is longer than the sentence buffer */
        if (++offset > bufsize) {
            fprintf(stderr, "Error reading sentence: \"%s...\"\n", sentencebuf);
            fprintf(stderr, "Set INDEXER_SENTENCE_SIZE to a bigger value to read longer sentences.\n");
            exit(1);
        }

        if (isspace(c)) {
            if (c == '\n' && line != NULL) {
                /* Count lines */
                (*line)++;
                newline++;
            }
            /* Set any whitespace to be the literal ' ', so that characters
                such as '\n' won't break the sentences */
            c = ' ';
        }
        else {
            newline = 0;
        }

        /* Add char to sentencebuf */
        *sentencebuf_pos++ = c;

        /* End of sentence if char is a puntuation or the second '\n' in a row */
        if (c == '.' || c == '!' || c == '?' || newline == 2) {
            break;
        }
    }
    *sentencebuf_pos = '\0';

    /* Remove leading whitespaces from a sentence, as well as repeating whitespaces */
    remove_redundant_spaces(sentencebuf);

    return c == EOF;
}

static char *getword(char *sentencebuf, char *wordbuf, int bufsize)
{
    char *sentencebuf_pos = sentencebuf;
    char *wordbuf_pos     = wordbuf;

    int offset = 0;
    char c;

    /* Put in `wordbuf` chars that aren't neither whitespaces nor punctuation chars, 
        except hyphen '-'; `isalpha()` isn't used because this function doesn't take into
        account non-ASCII characters, such that a word like "subúrbio" will be torn
        into "sub" and "rbio". */
    while (valid_in_word(c = *sentencebuf_pos++)) {
        /* Check whether the current word is longer than `wordbuf` */
        if (++offset > bufsize - 1) {
            fprintf(stderr, "Error reading word: \"%s...\"\n", wordbuf);
            fprintf(stderr, "Set INDEXER_WORD_BUFSIZE to a bigger value to read longer words.\n");
            exit(1);
        }

        c = tolower(c);
        *wordbuf_pos++ = c;
    }
    *wordbuf_pos = '\0';
    
    return sentencebuf_pos;
}

static int count_digits(int n)
{
    int count = 1;
    while ((n /= 10) > 0) count++;
    return count;
}

static int valid_in_word(char c)
{
    return (!isspace(c) && (!ispunct(c) || c == '-'));
}

static WordEntry *indexer_search(AvlTree *tree, char *word)
{
    WordEntry dummy = wentry_init(word, "", 0, "");
    WordEntry *wentry = avltree_search(tree, &dummy);
    wentry_destroy(&dummy);

    return wentry;
}

static void remove_redundant_spaces(char *s)
{
    char *sptr   = s;
    char *cursor = s;

    int whitespace = 1;
    while (*cursor != '\0') {
        /* This will be false when both *cursor and the character before it
            are whitespaces, or when *cursor is a leading whitespace */
        if (!isspace(*cursor) || !whitespace) {
            *sptr++ = *cursor
            whitespace = isspace(*cursor);
        }
    }
    *sptr = '\0';
}

static void dealloc_wentry(void *wentry)
{
    wentry_destroy(wentry);
}

static int compare_wentry(void *info, void *tree_data)
{
    WordEntry *wentry = info;
    WordEntry *tree_wentry = tree_data;
    return strcmp(wentry->word, tree_wentry->word);
}

#undef SIZEOFSTRING

#undef INDEXER_SENTENCE_BUFSIZE
#undef INDEXER_WORD_BUFSIZE

