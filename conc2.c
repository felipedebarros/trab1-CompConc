#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "timer.h"

#define STRING_SIZE 1048575
#define CHAR_ARRAY_SIZE 127
#define N 5
#define DEBUG 0

FILE *arq, *arqSaida;
int nThreads;
char* Buffer[N];
int count = 0, out = 0;
int fimDeArquivo = 0;

pthread_mutex_t mutex;
pthread_cond_t cond_cons, cond_prod;

void insereCaracteres(long int* cnt, char* s);
void merge(long int* a, long int* b);
void print(long int* c, FILE* arqSaida);

void insereBuffer(char* aux);
char* retiraBuffer(int n);

void* ProduzBloco(void* arg)
{
    size_t n = 0;

	double readStart, readEnd;
    GET_TIME(readStart);

    while(!feof(arq))
    {
        char* aux = (char*) malloc(sizeof(char) * (STRING_SIZE + 1));
        n = fread((void *)aux, sizeof(char), STRING_SIZE, arq);
        if(n < STRING_SIZE) aux[n] = '\0';
        insereBuffer(aux);
    }

    GET_TIME(readEnd);

    printf("Tempo de leitura: %lf\n", readEnd - readStart);

    pthread_mutex_lock(&mutex);
    fimDeArquivo = 1;
    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
}

void insereBuffer(char* aux)
{
    static int in = 0;
    pthread_mutex_lock(&mutex);
    while(count == N) {
        if(DEBUG) printf("Produtora vai se bloquear\n"); 
        pthread_cond_wait(&cond_prod, &mutex);
        if(DEBUG) printf("Produtora se livrou de suas amarras\n"); 
    }

    count++;
    Buffer[in] = aux;
    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&cond_cons);
    in = (in + 1) % N;
}

void* ConsomeBloco(void* arg)
{
    long int *temp_l = (long int*) malloc(sizeof(long int) * CHAR_ARRAY_SIZE);
    int i, tid = *(int *)arg; free(arg);
    for(i = 0; i < CHAR_ARRAY_SIZE; i++) temp_l[i] = 0;
    char* bloco;

    pthread_mutex_lock(&mutex);
    int fda = fimDeArquivo;
    int count_local = count;
    pthread_mutex_unlock(&mutex);

    while(!fda || count_local != 0)
    {
        bloco = retiraBuffer(tid);
        if(bloco != NULL)
        {
            insereCaracteres(temp_l, bloco);
            free(bloco);
        }

        pthread_mutex_lock(&mutex);
        fda = fimDeArquivo;
        count_local = count;
        pthread_mutex_unlock(&mutex);
    }

    pthread_cond_broadcast(&cond_cons);
    pthread_exit((void *)temp_l);
}

char* retiraBuffer(int n)
{
    pthread_mutex_lock(&mutex);
    while(count == 0)
    {
        if(fimDeArquivo)
        {
            if(DEBUG) printf("Consumidora %d retornou NULL\n", n);
            pthread_cond_broadcast(&cond_cons);
            pthread_mutex_unlock(&mutex);
            return NULL;
        }
        if(DEBUG) printf("Consumidora %d vai se bloquear\n", n);
        pthread_cond_wait(&cond_cons, &mutex);
        if(DEBUG) printf("Consumidora %d se livrou das amarras\n", n);
    }

    if(DEBUG) printf("Consumi %d\n", n);
    count--;
    char* bloco = Buffer[out];
    out = (out + 1) % N;

    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&cond_prod);

    return bloco;
}

int main(int argc, char** argv)
{
    int i;
    int *arg;
    pthread_t* system_id;
    long int charCount[CHAR_ARRAY_SIZE];
    double start, end;
    double procStart, procEnd;
    double printStart, printEnd;

    GET_TIME(start);

    for (i = 0; i < CHAR_ARRAY_SIZE; i++) charCount[i] = 0;

    if(argc < 4)
    {
        printf("Entrada deve ser da forma <arquivo de entrada>.txt <arquivo de saida>.txt <numero de consumidores>\n");
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
    
    nThreads = atoi(argv[3]) + 1;
    system_id = malloc(sizeof(pthread_t) * nThreads);
   
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond_prod, NULL);
    pthread_cond_init(&cond_cons, NULL);

    if(pthread_create(&system_id[0], NULL, ProduzBloco, NULL))
        { printf("Erro pthread_create"); exit(-1);}

    GET_TIME(procStart);
    for(i = 1; i < nThreads; i++)
    {
        arg = malloc(sizeof(int));
        *arg = i;
        if(pthread_create(&system_id[i], NULL, ConsomeBloco, (void*)arg))
        { printf("Erro pthread_create"); exit(-1);}
    }
    
    if(pthread_join(system_id[0], NULL))
        { printf("Erro pthread_join"); exit(-1);}

    if(DEBUG) printf("dei join (produtora)\n");

    long int* ret;
    for(i = 1; i < nThreads; i++)
    {
        if(pthread_join(system_id[i], (void**) &ret))
        { printf("Erro pthread_join"); exit(-1);}
        merge(charCount, ret);
    }
    GET_TIME(procEnd);
    printf("Tempo de processamento: %lf\n", procEnd - procStart);

    if(DEBUG) printf("dei join (consumidoras)\n");

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_cons);
    pthread_cond_destroy(&cond_prod);

    GET_TIME(printStart);
    fprintf(arqSaida ,"Caractere, Qtde\n");
    print(charCount, arqSaida);
    GET_TIME(printEnd);

    printf("Tempo de escrita: %lf\n", printEnd - printStart);

    fclose(arq);
    fclose(arqSaida);

    GET_TIME(end);
    printf("Tempo total decorrido: %lf\n", end - start);

    pthread_exit(NULL);
}

void insereCaracteres(long int* cnt, char* s)
{   
    int i;
    for (i = 0; i < STRING_SIZE; i++)
    {
    	if(s[i] == '\0') break;
        if(s[i] >= 0 && s[i] < CHAR_ARRAY_SIZE) cnt[s[i]]++;
    }
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
        || c == '.' || c == ';' 
        || c == ':' || c == '_' 
        || c == '-' || c == '(' 
        || c == ')' || c == '@' 
        || c == '%' || c == '&' 
        || c == '$' || c == '#' ) 
        return 1;
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