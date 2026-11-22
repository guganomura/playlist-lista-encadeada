#ifndef PLAYLIST_H
#define PLAYLIST_H

#include "lista.h"

Status inserir_no_inicio(Lista *lista, Musica musica);
Status inserir_no_final(Lista *lista, Musica musica);

/*
 * Retorna o primeiro no, de inicio em diante, cujo artista contem o termo, sem
 * diferenciar maiusculas de minusculas. Para o proximo resultado, chame de novo
 * com resultado->proximo. Assim nao e preciso outra estrutura para guarda-los.
 */
const No *buscar_por_artista(const No *inicio, const char *termo);

int quantidade_musicas(const Lista *lista);
long long duracao_total(const Lista *lista);

#endif
