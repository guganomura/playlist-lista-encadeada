/* Interface de terminal. As mensagens nao usam acentos para aparecerem
 * corretamente tambem no terminal do Windows. */
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lista.h"
#include "playlist.h"

#define TAM_LINHA 256

typedef enum { PERGUNTAR, INICIO, FINAL, POSICAO } Local;

static Lista playlist;

static void encerrar(void) {
    liberar_lista(&playlist);
}

/* ---------- Entrada ---------- */

/* Le uma linha e tira os espacos das pontas. */
static void ler_linha(const char *pergunta, char linha[TAM_LINHA]) {
    size_t inicio, fim;
    int c;

    for (;;) {
        printf("%s", pergunta);
        fflush(stdout);
        if (fgets(linha, TAM_LINHA, stdin) == NULL) {
            printf("\nPrograma encerrado.\n");
            exit(0);
        }
        if (strchr(linha, '\n') != NULL || feof(stdin)) break;

        while ((c = getchar()) != '\n' && c != EOF) {
            /* descarta o restante da linha */
        }
        printf("Entrada muito longa.\n");
    }
    inicio = strspn(linha, " \t\r\n");
    fim = strlen(linha);
    while (fim > inicio && strchr(" \t\r\n", linha[fim - 1]) != NULL) fim--;
    memmove(linha, linha + inicio, fim - inicio);
    linha[fim - inicio] = '\0';
}

static int ler_inteiro(const char *pergunta, int minimo, int maximo) {
    char linha[TAM_LINHA], *fim;
    long valor;

    for (;;) {
        ler_linha(pergunta, linha);
        errno = 0;
        valor = strtol(linha, &fim, 10);
        if (errno == 0 && fim != linha && *fim == '\0' && valor >= minimo && valor <= maximo)
            return (int)valor;
        printf("Digite um numero inteiro entre %d e %d.\n", minimo, maximo);
    }
}

static void ler_texto(const char *pergunta, char destino[TAM_TEXTO]) {
    char linha[TAM_LINHA];

    for (;;) {
        ler_linha(pergunta, linha);
        if (linha[0] == '\0') {
            printf("O campo nao pode ficar vazio.\n");
        } else if (strlen(linha) >= TAM_TEXTO) {
            printf("Texto muito longo (maximo de %d bytes).\n", TAM_TEXTO - 1);
        } else {
            strcpy(destino, linha);
            return;
        }
    }
}

/* Aceita de 00:01 a 999:59 e devolve em segundos. */
static int ler_duracao(void) {
    char linha[TAM_LINHA];
    const char *digitos = "0123456789";
    size_t m;
    int segundos;

    for (;;) {
        ler_linha("Duracao (mm:ss): ", linha);
        m = strspn(linha, digitos);
        if (m >= 1 && m <= 3 && linha[m] == ':' &&
            strspn(linha + m + 1, digitos) == 2 && linha[m + 3] == '\0' &&
            linha[m + 1] < '6') {
            segundos = atoi(linha) * 60 + atoi(linha + m + 1);
            if (segundos > 0) return segundos;
        }
        printf("Use o formato mm:ss, de 00:01 a 999:59.\n");
    }
}

static int confirmar(const char *pergunta) {
    char linha[TAM_LINHA];

    for (;;) {
        ler_linha(pergunta, linha);
        if (strcmp(linha, "s") == 0 || strcmp(linha, "S") == 0) return 1;
        if (strcmp(linha, "n") == 0 || strcmp(linha, "N") == 0) return 0;
        printf("Responda com s ou n.\n");
    }
}

static Musica ler_musica(void) {
    Musica musica;

    for (;;) {
        musica.id = ler_inteiro("ID: ", 1, INT_MAX);
        if (buscar(&playlist, musica.id) == NULL) break;
        printf("Ja existe uma musica com ID %d.\n", musica.id);
    }
    ler_texto("Titulo: ", musica.titulo);
    ler_texto("Artista: ", musica.artista);
    ler_texto("Album: ", musica.album);
    musica.duracao = ler_duracao();
    return musica;
}

/* ---------- Opcoes do menu ---------- */

static void opcao_inicializar(void) {
    if (playlist.quantidade > 0 && !confirmar("Apagar a playlist atual? (s/n): ")) {
        printf("Playlist mantida.\n");
        return;
    }
    liberar_lista(&playlist); /* evita vazamento dos nos antigos */
    inicializar_lista(&playlist);
    printf("Playlist vazia inicializada.\n");
}

static void opcao_inserir(Local local) {
    Musica musica = ler_musica();
    Status status;
    int posicao;

    if (local == PERGUNTAR)
        local = (Local)ler_inteiro("Onde inserir? 1-inicio 2-final 3-posicao: ", INICIO, POSICAO);

    if (local == INICIO) {
        posicao = 1;
        status = inserir_no_inicio(&playlist, musica);
    } else if (local == FINAL) {
        posicao = playlist.quantidade + 1;
        status = inserir_no_final(&playlist, musica);
    } else {
        /* O usuario conta a partir de 1 e a lista a partir de 0. */
        posicao = ler_inteiro("Posicao (1 = inicio): ", 1, playlist.quantidade + 1);
        status = inserir(&playlist, musica, posicao - 1);
    }

    /* ID e posicao ja foram validados, entao so a memoria pode falhar. */
    if (status == OK) printf("Musica inserida na posicao %d.\n", posicao);
    else printf("Erro: memoria insuficiente.\n");
}

static void opcao_buscar(void) {
    int id = ler_inteiro("ID procurado: ", 1, INT_MAX);
    const No *no = buscar(&playlist, id);

    if (no == NULL) {
        printf("Nenhuma musica com ID %d.\n", id);
    } else {
        printf("Encontrada: ");
        imprimir_musica(&no->musica, stdout);
    }
}

static void opcao_buscar_artista(void) {
    char termo[TAM_TEXTO];
    const No *no;
    int encontradas = 0;

    ler_texto("Artista procurado: ", termo);
    for (no = buscar_por_artista(playlist.primeiro, termo); no != NULL;
         no = buscar_por_artista(no->proximo, termo)) {
        printf("  ");
        imprimir_musica(&no->musica, stdout);
        encontradas++;
    }
    printf("%d musica(s) encontrada(s).\n", encontradas);
}

static void opcao_remover(void) {
    int id = ler_inteiro("ID a remover: ", 1, INT_MAX);
    Musica removida;

    if (remover(&playlist, id, &removida) != OK) {
        printf("Nenhuma musica com ID %d.\n", id);
    } else {
        printf("Removida: ");
        imprimir_musica(&removida, stdout);
    }
}

static void opcao_duracao(void) {
    char texto[TAM_DURACAO];

    formatar_duracao(duracao_total(&playlist), texto);
    printf("Duracao total: %s\n", texto);
}

static void mostrar_menu(void) {
    printf("\n=== PLAYLIST ===\n"
           " 1. Inicializar playlist vazia\n"
           " 2. Cadastrar nova musica\n"
           " 3. Inserir no inicio\n"
           " 4. Inserir no final\n"
           " 5. Inserir em uma posicao\n"
           " 6. Exibir playlist\n"
           " 7. Buscar por ID\n"
           " 8. Buscar por artista\n"
           " 9. Remover por ID\n"
           "10. Quantidade de musicas\n"
           "11. Duracao total\n"
           " 0. Sair\n");
}

int main(void) {
    int opcao;

    inicializar_lista(&playlist);
    atexit(encerrar);

    for (;;) {
        mostrar_menu();
        opcao = ler_inteiro("Opcao: ", 0, 11);

        switch (opcao) {
            case 0: printf("Programa encerrado.\n"); return 0;
            case 1: opcao_inicializar(); break;
            case 2: opcao_inserir(PERGUNTAR); break;
            case 3: opcao_inserir(INICIO); break;
            case 4: opcao_inserir(FINAL); break;
            case 5: opcao_inserir(POSICAO); break;
            case 6: imprimir(&playlist, stdout); break;
            case 7: opcao_buscar(); break;
            case 8: opcao_buscar_artista(); break;
            case 9: opcao_remover(); break;
            case 10: printf("Quantidade de musicas: %d\n", quantidade_musicas(&playlist)); break;
            case 11: opcao_duracao(); break;
        }
    }
}
