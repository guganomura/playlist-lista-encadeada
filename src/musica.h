#ifndef MUSICA_H
#define MUSICA_H

#include <stdio.h>

#define TAM_TEXTO 100
#define TAM_DURACAO 32

typedef struct {
    int id;
    char titulo[TAM_TEXTO];
    char artista[TAM_TEXTO];
    char album[TAM_TEXTO];
    int duracao; /* em segundos */
} Musica;

/* Escreve mm:ss, ou h:mm:ss a partir de uma hora. */
void formatar_duracao(long long segundos, char texto[TAM_DURACAO]);

void imprimir_musica(const Musica *musica, FILE *saida);

#endif
