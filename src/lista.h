#ifndef LISTA_H
#define LISTA_H

#include "musica.h"

typedef struct No {
    Musica musica;
    struct No *proximo; /* NULL no ultimo no */
} No;

typedef struct {
    No *primeiro; /* NULL se a lista estiver vazia */
    int quantidade;
} Lista;

typedef enum {
    OK,
    ERRO_ID_REPETIDO,
    ERRO_POSICAO,
    ERRO_MEMORIA,
    ERRO_NAO_ENCONTRADA
} Status;

/* Deixa a lista vazia. Se ela ja tiver nos, chame liberar_lista antes. */
void inicializar_lista(Lista *lista);

/* Insere uma copia da musica. posicao vai de 0 (inicio) a quantidade (fim). */
Status inserir(Lista *lista, Musica musica, int posicao);

void imprimir(const Lista *lista, FILE *saida);

/* Retorna o no que tem o ID, ou NULL. */
No *buscar(const Lista *lista, int id);

/* Se removida nao for NULL, recebe a copia da musica que saiu. */
Status remover(Lista *lista, int id, Musica *removida);

/* Auxiliar exigida pelo C: devolve a memoria de todos os nos. */
void liberar_lista(Lista *lista);

#endif
