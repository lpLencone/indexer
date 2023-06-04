/* Integrantes do grupo: 
	Lucas Plaza
	Jefferson Dutra
	Felipe Coutinho	*/

#include "indexer/indexer.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#ifndef _INDEXER_WORD_BUFSIZE
# define _INDEXER_WORD_BUFSIZE 32
#endif

int main(int argc, char **argv)
{
	if (argc < 2) {
		fprintf(stderr, "Erro de uso: %s arquivo_1 [, arquivo_2 [, ...]]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/* argv + 1: posição no vetor argv em que os arquivos são recebidos */
	/* argc - 1: quantidade de arquivos passados para o programa */
	Indexer indexer = indexer_init(argv + 1, argc - 1);
	
	char c;
	char wordbuf[_INDEXER_WORD_BUFSIZE];
	char *wordbuf_ptr;
	int n_occurrences_file;
	
	char occurrences_buf[4];
	while (1) {
		wordbuf_ptr = wordbuf;

		printf("Entre com uma palavra ou insira \"--sair\" para encerrar o programa.\n");
		printf("Palavra: ");

		while ((c = getchar()) != '\n') {
			c = tolower(c);
        	*wordbuf_ptr++ = c;
		}
		*wordbuf_ptr = '\0';

		if (strcmp(wordbuf, "--sair") == 0) {
			break;
		}

		printf("Quantas ocorrencias por arquivo: ");
		fgets(occurrences_buf, sizeof(char) * 4, stdin);
		n_occurrences_file = atoi(occurrences_buf);

		printf("\n");

		indexer_print(&indexer, wordbuf, n_occurrences_file);
	}
	

	indexer_destroy(&indexer);

	return 0;
}

