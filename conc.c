#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "timer.h"

#define STRING_SIZE 1023
#define CHAR_ARRAY_SIZE 127

FILE *arq, *arqSaida;
int nThreads;

pthread_mutex_t arq_mutex;

void insereCaracteres(long int* cnt, char* s);
void merge(long int* a, long int* b);
void print(long int* c, FILE* arq);

void* ContaChar(void* arg)
{
    char aux[STRING_SIZE + 1];
    size_t n = 0;
    long int temp_l[CHAR_ARRAY_SIZE];
    int i, tid = *(int *)arg;
    for(i = 0; i < CHAR_ARRAY_SIZE; i++) temp_l[i] = 0;
    while(!feof(arq))
    {
        pthread_mutex_lock(&arq_mutex);
        n = fread((void *)aux, sizeof(char), STRING_SIZE, arq);
        pthread_mutex_unlock(&arq_mutex);
        if(n < STRING_SIZE) aux[n] = '\0';
        insereCaracteres(temp_l, aux);
        //printf("Thread: %d %d\n", tid, (int) n);
    }
    pthread_exit((void *)temp_l);
}

int main(int argc, char** argv)
{
    int i, arg;
    pthread_t* system_id;
    long int charCount[CHAR_ARRAY_SIZE];
    double start, end;

    for (i = 0; i < CHAR_ARRAY_SIZE; i++) charCount[i] = 0;

    if(argc < 4)
    {
        printf("Entrada deve ser da forma <arquivo de entrada>.txt <arquivo de saida>.txt <numero de threads>\n");
        exit(-1);
    }

    arq = fopen(argv[1], "r");
    if(arq == NULL) {
      fprintf(stderr, "Erro ao abrir o arquivo de entrada.\n");
      exit(-1);
    }

    arqSaida = fopen(argv[2], "w");
    if(arqSaida == NULL) {
      fprintf(stderr, "Erro ao abrir o arquivo de saida.\n");
      exit(-1);
    }
    
    nThreads = atoi(argv[3]);
    system_id = malloc(sizeof(pthread_t) * nThreads);
    
    GET_TIME(start);

    pthread_mutex_init(&arq_mutex, NULL);
    
    for(i = 0; i < nThreads; i++)
    {
        arg = i;
        if(pthread_create(&system_id[i], NULL, ContaChar, (void*)&arg))
        { printf("Erro pthread_create"); exit(-1);}
    }
    
    long int* ret;
    for(i = 0; i < nThreads; i++)
    {
        if(pthread_join(system_id[i], (void**) &ret))
        { printf("Erro pthread_join"); exit(-1);}
        merge(charCount, ret);
    }

    pthread_mutex_destroy(&arq_mutex);

    GET_TIME(end);

    printf("Caractere, Qtde\n");
    print(charCount, arqSaida);
    printf("Tempo decorrido %lf\n", end - start);

    fclose(arq);
    fclose(arqSaida);

    pthread_exit(NULL);
}

void insereCaracteres(long int* cnt, char* s)
{   
    int i, size = strlen(s);
    for (i = 0; i < size; i++)
        if(s[i] >= 0 && s[i] < CHAR_ARRAY_SIZE) cnt[s[i]]++;
}

void merge(long int* a, long int* b)
{
    int i;
    for(i = 0; i < CHAR_ARRAY_SIZE; i++)
        a[i] += b[i];
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
        || c == '#' ) return 1;
    return 0;
}

void print(long int* c, FILE* arq)
{
    int i;
    for (i = 0; i < CHAR_ARRAY_SIZE; i++)
    {
        if(checkChar(i) && c[i] > 0) fprintf(arq, "%c, %ld\n", i, c[i]);
    }
}