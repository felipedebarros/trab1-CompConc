#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "map.h"

#define STRING_SIZE 2047

Map* map;
FILE* arq;

Map* insereCaracteres(char* s);

int main(int argc, char** argv)
{
	int i;
	char aux[STRING_SIZE + 1];

	map = (Map*) malloc(sizeof(Map));
	initialize(map);

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

  	char buffer[128];
	sprintf(buffer, "%%%dc", STRING_SIZE);
	while(!feof(arq))
	{
		fscanf(arq, buffer, aux);
		Map* temp_m = insereCaracteres(aux);
		merge(map, temp_m);
		strcpy(aux, "");
	}


	printf("Caractere, Qtde\n");
	print(map);

	destroy(map);

	return 0;
}

Map* insereCaracteres(char* s)
{
	Map* map = (Map*) malloc(sizeof(Map));
	initialize(map);
	int i, size = strlen(s) - 1;
	for (i = 0; i < size; i++)
		insert(map, s[i]);
	return map;
}