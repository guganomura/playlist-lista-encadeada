#include "musica.h"

void formatar_duracao(long long segundos, char texto[TAM_DURACAO]) {
    long long horas = segundos / 3600;
    int minutos = (int)(segundos % 3600 / 60);
    int resto = (int)(segundos % 60);

    if (horas > 0) snprintf(texto, TAM_DURACAO, "%lld:%02d:%02d", horas, minutos, resto);
    else snprintf(texto, TAM_DURACAO, "%02d:%02d", minutos, resto);
}

void imprimir_musica(const Musica *musica, FILE *saida) {
    char duracao[TAM_DURACAO];
    formatar_duracao(musica->duracao, duracao);
    fprintf(saida, "ID %d | %s | %s | %s | %s\n",
            musica->id, musica->titulo, musica->artista, musica->album, duracao);
}
