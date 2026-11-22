#include "lista.h"

#include <stdlib.h>

void inicializar_lista(Lista *lista) {
    lista->primeiro = NULL;
    lista->quantidade = 0;
}

Status inserir(Lista *lista, Musica musica, int posicao) {
    No *novo, *anterior;
    int i;

    if (posicao < 0 || posicao > lista->quantidade) return ERRO_POSICAO;
    if (buscar(lista, musica.id) != NULL) return ERRO_ID_REPETIDO;

    novo = malloc(sizeof *novo);
    if (novo == NULL) return ERRO_MEMORIA;
    novo->musica = musica;

    if (posicao == 0) {
        /* Inicio: o novo aponta para o antigo primeiro e toma o lugar dele. */
        novo->proximo = lista->primeiro;
        lista->primeiro = novo;
    } else {
        /* Meio ou fim: anda ate o no que ficara antes do novo. */
        anterior = lista->primeiro;
        for (i = 0; i < posicao - 1; i++) {
            anterior = anterior->proximo;
        }
        /* A ordem importa: se invertida, perderiamos o restante da lista. */
        novo->proximo = anterior->proximo;
        anterior->proximo = novo;
    }

    lista->quantidade++;
    return OK;
}

void imprimir(const Lista *lista, FILE *saida) {
    const No *atual;
    int posicao = 1;

    fprintf(saida, "Playlist: ");
    for (atual = lista->primeiro; atual != NULL; atual = atual->proximo) {
        fprintf(saida, "[%s] -> ", atual->musica.titulo);
    }
    fprintf(saida, "NULL\n");

    for (atual = lista->primeiro; atual != NULL; atual = atual->proximo) {
        fprintf(saida, "  %d. ", posicao++);
        imprimir_musica(&atual->musica, saida);
    }
}

No *buscar(const Lista *lista, int id) {
    No *atual = lista->primeiro;

    while (atual != NULL) {
        if (atual->musica.id == id) return atual;
        atual = atual->proximo;
    }
    return NULL;
}

Status remover(Lista *lista, int id, Musica *removida) {
    No *anterior = NULL;
    No *atual = lista->primeiro;

    while (atual != NULL && atual->musica.id != id) {
        anterior = atual;
        atual = atual->proximo;
    }
    if (atual == NULL) return ERRO_NAO_ENCONTRADA;

    if (anterior == NULL) {
        /* E o primeiro: a lista passa a comecar no segundo (ou em NULL). */
        lista->primeiro = atual->proximo;
    } else {
        /* Meio ou fim: o anterior pula o no removido. */
        anterior->proximo = atual->proximo;
    }

    if (removida != NULL) *removida = atual->musica;
    free(atual);
    lista->quantidade--;
    return OK;
}

void liberar_lista(Lista *lista) {
    No *atual = lista->primeiro;
    No *seguinte;

    while (atual != NULL) {
        seguinte = atual->proximo; /* guarda antes do free */
        free(atual);
        atual = seguinte;
    }
    inicializar_lista(lista);
}
