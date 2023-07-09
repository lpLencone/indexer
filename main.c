#include "indexer/indexer.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#ifndef INDEXER_WORD_BUFSIZE
# define INDEXER_WORD_BUFSIZE 32
#endif

int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "Usage error: %s file_1 [file_2 [...]]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* argv + 1: argv position the files start lining up */
    /* argv - 1: number of files received */
    Indexer indexer = indexer_init(argv + 1, argc - 1);
    
    char c;
    char wordbuf[INDEXER_WORD_BUFSIZE];
    char *wordbuf_pos;
    int n_occurrences_file;
    
    char occurrences_buf[8];
    while (1) {
        wordbuf_pos = wordbuf;

        printf("Enter a word or \"--exit\" to exit the program.\n");
        printf("Word: ");

        int bufpos = 0;

        while ((c = getchar()) != '\n') {
            
            if (++bufpos > INDEXER_WORD_BUFSIZE - 1) {
                fprintf(stderr, "Error reading word: \"%s...\"\n", wordbuf);
                fprintf(stderr, "Set INDEXER_WORD_BUFSIZE to a bigger value to be able to read bigger words\n");
                exit(1);
            }

            c = tolower(c);
            *wordbuf_pos++ = c;
        }
        *wordbuf_pos = '\0';

        if (strcmp(wordbuf, "--exit") == 0) {
            break;
        }

        printf("Print how many occurrences from each file: ");
        fgets(occurrences_buf, 8, stdin);
        n_occurrences_file = atoi(occurrences_buf);

        printf("\n");

        indexer_print(&indexer, wordbuf, n_occurrences_file);
    }
    

    indexer_destroy(&indexer);

    return 0;
}

