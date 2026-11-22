#include "playlist.h"

#include <string.h>

Status inserir_no_inicio(Lista *lista, Musica musica) {
    return inserir(lista, musica, 0);
}

Status inserir_no_final(Lista *lista, Musica musica) {
    return inserir(lista, musica, lista->quantidade);
}

/*
 * Converte para minusculas. Em UTF-8, as maiusculas acentuadas do portugues sao
 * o byte 0xC3 seguido de 0x80 a 0x9E, e as minusculas ficam 0x20 acima (0x97 e
 * o sinal de multiplicacao, que nao muda).
 */
static void para_minusculas(char *destino, const char *origem) {
    unsigned char anterior = 0, c;
    int i;

    for (i = 0; origem[i] != '\0' && i < TAM_TEXTO - 1; i++) {
        c = (unsigned char)origem[i];
        if (c >= 'A' && c <= 'Z') c += 'a' - 'A';
        else if (anterior == 0xC3 && c >= 0x80 && c <= 0x9E && c != 0x97) c += 0x20;
        destino[i] = (char)c;
        anterior = c;
    }
    destino[i] = '\0';
}

const No *buscar_por_artista(const No *inicio, const char *termo) {
    char procurado[TAM_TEXTO], artista[TAM_TEXTO];
    const No *atual;

    if (termo[0] == '\0') return NULL;
    para_minusculas(procurado, termo);

    for (atual = inicio; atual != NULL; atual = atual->proximo) {
        para_minusculas(artista, atual->musica.artista);
        if (strstr(artista, procurado) != NULL) return atual;
    }
    return NULL;
}

int quantidade_musicas(const Lista *lista) {
    return lista->quantidade;
}

long long duracao_total(const Lista *lista) {
    const No *atual;
    long long total = 0;

    for (atual = lista->primeiro; atual != NULL; atual = atual->proximo) {
        total += atual->musica.duracao;
    }
    return total;
}
