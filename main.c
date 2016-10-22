#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "timer.h"

#define STRING_SIZE 511
#define CHAR_ARRAY_SIZE 127

long int charCount[CHAR_ARRAY_SIZE];
FILE* arq;

long int* insereCaracteres(char* s);
void merge(long int* a, long int* b);
void print(long int* c);

int main(int argc, char** argv)
{
	int i;
	char aux[STRING_SIZE + 1];
	double start, end;

	for (i = 0; i < CHAR_ARRAY_SIZE; i++) charCount[i] = 0;

	if(argc < 2)
	{
		printf("Entrada deve ser da forma <arquivo de entrada>.txt\n");
		exit(-1);
	}

	arq = fopen(argv[1], "r");
	if(arq == NULL) {
      fprintf(stderr, "Erro ao abrir o arquivo de entrada.\n");
      exit(-1);
  	}

  	GET_TIME(start);
	size_t n = 0;
	long int* temp_l;
	while(!feof(arq))
	{
		n = fread((void *)aux, 1, STRING_SIZE, arq);
		if(n < STRING_SIZE) aux[n] = '\0';
		temp_l = insereCaracteres(aux);
		merge(charCount, temp_l);
	}
	GET_TIME(end);

	printf("Caractere, Qtde\n");
	print(charCount);
	printf("Tempo decorrido: %lf\n", end - start);

	return 0;
}

long int* insereCaracteres(char* s)
{	
	long int* cnt = malloc(sizeof(long int) * CHAR_ARRAY_SIZE);
	int i, size = strlen(s);
	for (i = 0; i < CHAR_ARRAY_SIZE; i++) cnt[i] = 0;
	for (i = 0; i < size; i++)
		if(s[i] >= 0 && s[i] < CHAR_ARRAY_SIZE) cnt[s[i]]++;
	return cnt;
}

void merge(long int* a, long int* b)
{
	int i;
	for(i = 0; i < CHAR_ARRAY_SIZE; i++)
		a[i] += b[i];
	free(b);
}

int checkChar(char c)
{
	if(c >= 'A' && c <= 'Z') return 1;
	if(c >= 'a' && c <= 'z') return 1;
	if(c >= '0' && c <= '9') return 1;
	if(c == '?' || c == '!'
		|| c == '.' || c == ','
		|| c == ';' || c == ':'
		|| c == '_' || c == '-'
		|| c == '(' || c == ')'
		|| c == '@' || c == '%'
		|| c == '&' || c == '$'
		|| c == '#') return 1;
	return 0;
}

void print(long int* c)
{
	int i;
	for (i = 0; i < CHAR_ARRAY_SIZE; i++)
	{
		if(checkChar(i) && c[i] > 0) printf("%c, %ld\n", i, c[i]);
	}
	printf("\n");
}