#include "indexer/indexer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Para ler frase com mais de 1028 caracteres, compilar definindo _INDEXER_SENTENCE_BUFSIZE
    para um valor maior */
#ifndef _INDEXER_SENTENCE_BUFSIZE
# define _INDEXER_SENTENCE_BUFSIZE 1028
#endif
/* Para ler palavras com mais de 32 caracteres, compilar definindo _INDEXER_WORD_BUFSIZE
    para um valor maior */
#ifndef _INDEXER_WORD_BUFSIZE
# define _INDEXER_WORD_BUFSIZE 32
#endif

static void     wordindexer(char *filename, AvlTree *tree);

static void	    updateword(AvlTree *tree, char *word, char *sentence, int line, char *filename);

/* Lê uma cadeia de caracteres de `file` para `sentencebuf` até a primeira aparição
    de um '.' (período), '?' (ponto de interrogação), ou '!' (ponto de exclamanção) */
static int      getsentence(FILE *file, char *sentencebuf, int bufsize, int *line);

/* Coloca uma palavra de `sentencebuf` em `wordbuf` e retorna um ponteiro
    para o próximo caracter não lido de `sentencebuf` */
static char	    *getword(char *sentencebuf, char *wordbuf, int bufsize);

/* Verifica se o caracter `c` é válido dentro de uma palavra. */
static int      valid_in_word(char c);

/* Conta a quantidade de algarítmos em um número */
static int      count_digits(int n);

/* Remove espaços que aparecem antes de algum caracter alfabético e múltiplos
    espaços que aparecem entre palavras. Por espaço, esse programa considera
    a definição adotada pela biblioteca ctypes.h, com o uso do `isspace()` */
static void     remove_redundant_spaces(char *s);

static void     dealloc_wentry(void *wentry);
static int      compare_wentry(void *info, void *tree_data);

/* Procura por um registro que contenha `word` na árvore; se não existir
    retorna NULL. */
static WordEntry *indexer_search(AvlTree *tree, char *word);

/* Tornar o código mais legível. */
#define SIZEOFSTRING(str) (sizeof(char) * (strlen(str) + 1))

Indexer indexer_init(char **filenames, int n_files)
{
    Indexer indexer;
    indexer.n_files = n_files;

    indexer.filenames = (char **)malloc(n_files * sizeof(char *));
    for (int i = 0; i < n_files; i++) {
        indexer.filenames[i] = (char *)malloc(SIZEOFSTRING(filenames[i]));
        strcpy(indexer.filenames[i], filenames[i]);
        printf("Arquivo adicionado: %s\n", indexer.filenames[i]);
    }

    printf("Todos os arquivos foram adicionados.\n");
    printf("===================================\n\n");

    indexer.wentries = avltree_init(dealloc_wentry, compare_wentry);
    for (int i = 0; i < n_files; i++) {
        printf("Lendo arquivo: %s\n", indexer.filenames[i]);
        wordindexer(indexer.filenames[i], &indexer.wentries);
    }

    printf("Arquivos foram lidos com sucesso.\n");
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
    /* Procurar palavra na árvore */
    WordEntry *wentry = indexer_search(&indexer->wentries, word);

    /* Se a palavra existir */
    if (wentry != NULL) {
        /* Imprimir a palavra e a quantidade total de ocorrências */
        printf("Ocorrencia total: %d\n", wentry->total);

        /* Cursar pela lista de arquivos em que a palavra aparece */
        Node *fentry_cursor = wentry->fentries.first;
        while (fentry_cursor != NULL) {
            FileEntry *fentry = (FileEntry *)fentry_cursor->data;
            
            /* Imprimir nome do arquivo e a quantidade de ocorrências no arquivo */
            printf("--| Arquivo: %s\n", fentry->filename);
            printf("--| Ocorrencias: %d\n", fentry->frequency);

            /* Criar formatação para as ocorrências nesse arquivo a partir da quantidade
                de linhas que ele possui. */
            char occurrence_format[64];
            int line_digits = count_digits(fentry->n_lines);
            int occurrence_digits = count_digits(n_occurrences_file);
            sprintf(occurrence_format, "[%%%dd]-[%%%dd] %%s\n", occurrence_digits, line_digits);
            
            int occurrence_num = 1;
            /* Cursar pela lista de ocorrências da palavra no arquivo e imprimir
                as frases em que a palavra ocorre */
            Node *occurrence_cursor = fentry->occurrences.first;
            while (occurrence_cursor != NULL && occurrence_num <= n_occurrences_file) {
                Occurrence *occurrence = (Occurrence *)occurrence_cursor->data;
                printf(occurrence_format, occurrence_num++, occurrence->line, occurrence->sentence);

                occurrence_cursor = occurrence_cursor->next;    
            }

            fentry_cursor = fentry_cursor->next;
            printf("\n");
        }
        printf("\n===================================\n\n");
    }
    /* Se a palavra não exisitr */
    else {
        printf("Palavra \"%s\" nao existe em nenhum arquivo\n", word);
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
        perror("Erro ao tentar abrir arquivo");
        exit(1);
    }

	char sentencebuf[_INDEXER_SENTENCE_BUFSIZE];
    char wordbuf[_INDEXER_WORD_BUFSIZE];
	
    int end_of_file;
    int line = 1;
    /* Obtém todas as frases de um arquivo até o seu fim. */
    do {
        /* Salva a linha em que a frase iniciou */
        int sentence_start_line = line;
        end_of_file = getsentence(file, sentencebuf, _INDEXER_SENTENCE_BUFSIZE, &line);

        /* Se uma frase válida foi obtida */
        if (strlen(sentencebuf) > 1) {
			char *sentencebuf_ptr = sentencebuf;

            /* Obtém todas as palavras em uma frase */
			do {
				sentencebuf_ptr = getword(sentencebuf_ptr, wordbuf, _INDEXER_WORD_BUFSIZE);

                /* Se uma palavra válida foi obtida, atualize suas informações
                    nos registros presentes na árvore AVL */
                if (strlen(wordbuf) > 0) {
				    updateword(tree, wordbuf, sentencebuf, sentence_start_line, filename);
                }
			}
			while (*sentencebuf_ptr != '\0');
        }
    }
    while (!end_of_file);

	fclose(file);
}

static void updateword(AvlTree *tree, char *word, char *sentence, int line, char *filename)
{
    /* Procura pela palavra na árvore */
    WordEntry *wentry = indexer_search(tree, word);

    if (wentry != NULL) {
        wentry_update(wentry, sentence, line, filename);
    }
    else {
        /* Crie novo registro de palavra se a palavra ainda não existia na árvore. */
		WordEntry new_wentry = wentry_init(word, sentence, line, filename);
        avltree_insert(tree, (void *)&new_wentry, sizeof(new_wentry));
    }
}

static int getsentence(FILE *file, char *sentencebuf, int bufsize, int *line)
{
    char *sentencebuf_ptr = sentencebuf;

    char c;
    int bufpos = 0;
    int newline = 0;

    while ((c = fgetc(file)) != EOF) {
        /* Checar por leituras maiores que o buffer de frase */
        if (++bufpos > bufsize) {
            fprintf(stderr, "Erro lendo frase: \"%s...\"\n", sentencebuf);
            fprintf(stderr, "Por favor, aumente o tamanho do buffer definindo _INDEXER_SENTECE_SIZE "
                            "para um tamanho que comporte frases maiores.");
            exit(1);
        }

        if (isspace(c)) {
            if (c == '\n' && line != NULL) {
                /* Conta linhas */
                (*line)++;
                newline++;
            }

            /* Transforma qualquer caracter 'espaço' para o literal ' '
                para caracteres como '\n' não quebrar as frases no meio */
            c = ' ';
        }
        else {
            newline = 0;
        }

        /* Adiciona o caracter ao buffer da frase */
        *sentencebuf_ptr++ = c;

        /* Encerre o loop se o fim da frase chegar, seja por pontuação terminal,
            ou por duas quebras de linha seguidas */
		if (c == '.' || c == '!' || c == '?' || newline == 2) {
			break;
		}
    }
	*sentencebuf_ptr = '\0';

    /* Remove espaços que lideram uma frase, e também espaços repetidos no meio
        de uma frase. */
    remove_redundant_spaces(sentencebuf);

    return (c == EOF) ? 1 : 0;
}

static char *getword(char *sentencebuf, char *wordbuf, int bufsize)
{
	char *sentencebuf_ptr = sentencebuf;
	char *wordbuf_ptr     = wordbuf;

    int bufpos = 0;
    char c;
    /* Coloca em wordbuf carcteres que não são espaços nem pontuações, com a exceção
        do hífen '-'; a razão pela qual `isalpha()` não é utilizado é porque essa função
        não inclui caracteres não ASCII, de modo que uma palavra como "subúrbio" seja
        quebrada em duas partes "sub" e "rbio". */
    while (valid_in_word(c = *sentencebuf_ptr++)) {
        /* Checar por leituras maiores que o buffer da palavra */
        if (++bufpos > bufsize - 1) {
            fprintf(stderr, "Erro lendo palavra: \"%s...\"\n", wordbuf);
            fprintf(stderr, "Por favor, aumente o tamanho do buffer definindo _INDEXER_WORD_SIZE "
                            "para um tamanho que comporte palavras maiores.");
            exit(1);
        }

        c = tolower(c);
        *wordbuf_ptr++ = c;
    }
    *wordbuf_ptr = '\0';
    
    return sentencebuf_ptr;
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
    WordEntry *wentry = (WordEntry *)avltree_search(tree, &dummy);
    wentry_destroy(&dummy);

    return wentry;
}

static void remove_redundant_spaces(char *s)
{
    char *sptr, *cursor;
    sptr = cursor = s;

    int spacechar = 1;
    while (*cursor != '\0') {
        if (!spacechar) {
            if (isspace(*sptr++ = *cursor++)) {
                spacechar = 1;
            }
        }
        else {
            if (!isspace(*cursor)) {
                spacechar = 0;
            }
            else {
                cursor++;
            }
        }
    }
    *sptr = '\0';
}

static void dealloc_wentry(void *wentry)
{
	wentry_destroy((WordEntry *)wentry);
}

static int compare_wentry(void *info, void *tree_data)
{
	WordEntry *wentry = (WordEntry *)info;
    WordEntry *tree_wentry = (WordEntry *)tree_data;
	return strcmp(wentry->word, tree_wentry->word);
}

#undef SIZEOFSTRING

#undef _INDEXER_SENTENCE_BUFSIZE
#undef _INDEXER_WORD_BUFSIZE

