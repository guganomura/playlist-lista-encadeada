#include <limits.h>
#include <string.h>

#include "../src/lista.h"
#include "../src/playlist.h"

static int total, falhas;

static void verificar(const char *id, const char *cenario, const char *esperado, int condicao) {
    total++;
    if (!condicao) falhas++;
    printf("[%s] %s %s -> %s\n", condicao ? "OK" : "FALHA", id, cenario, esperado);
}

static Musica musica(int id, const char *artista, int duracao) {
    Musica m;
    m.id = id;
    snprintf(m.titulo, sizeof m.titulo, "M%d", id % 1000);
    strcpy(m.artista, artista);
    strcpy(m.album, "Album");
    m.duracao = duracao;
    return m;
}

/* Cria uma lista com IDs de 1 a n, nessa ordem, cada musica com id * 60 s. */
static Lista criar(int n) {
    Lista l;
    int i;
    inicializar_lista(&l);
    for (i = 1; i <= n; i++) inserir(&l, musica(i, "Artista", i * 60), i - 1);
    return l;
}

/* Confere ordem, quantidade e o NULL final, com ids como "123". */
static int ordem(const Lista *l, const char *ids) {
    const No *no = l->primeiro;
    int n = 0;
    for (; ids[n] != '\0'; n++) {
        if (no == NULL || no->musica.id != ids[n] - '0') return 0;
        no = no->proximo;
    }
    return no == NULL && l->quantidade == n;
}

static int impresso(const Lista *l, const char *esperado) {
    char texto[512];
    FILE *f = tmpfile();
    size_t n;
    if (f == NULL) return 0;
    imprimir(l, f);
    rewind(f);
    n = fread(texto, 1, sizeof texto - 1, f);
    texto[n] = '\0';
    fclose(f);
    return strcmp(texto, esperado) == 0;
}

static void testar_obrigatorios(void) {
    Lista l;
    Musica m;
    const No *no;

    inicializar_lista(&l);
    verificar("CT-01", "criacao de playlist vazia", "primeiro = NULL, quantidade = 0",
              l.primeiro == NULL && l.quantidade == 0);

    verificar("CT-02", "insercao da primeira musica", "[1] -> NULL",
              inserir(&l, musica(1, "A", 60), 0) == OK && ordem(&l, "1"));
    liberar_lista(&l);

    l = criar(5);
    verificar("CT-03", "insercao de varias musicas", "[1] a [5], em ordem", ordem(&l, "12345"));
    liberar_lista(&l);

    l = criar(3);
    verificar("CT-04", "insercao de 4 no inicio de [1,2,3]", "[4,1,2,3]",
              inserir_no_inicio(&l, musica(4, "A", 60)) == OK && ordem(&l, "4123"));
    liberar_lista(&l);

    l = criar(3);
    verificar("CT-05", "insercao de 4 no final de [1,2,3]", "[1,2,3,4]",
              inserir_no_final(&l, musica(4, "A", 60)) == OK && ordem(&l, "1234"));
    liberar_lista(&l);

    l = criar(4);
    verificar("CT-06", "insercao de 5 no meio de [1,2,3,4]", "[1,2,5,3,4]",
              inserir(&l, musica(5, "A", 60), 2) == OK && ordem(&l, "12534"));
    liberar_lista(&l);

    l = criar(3);
    no = buscar(&l, 2);
    verificar("CT-07", "busca do ID 2, existente", "retorna o segundo no",
              no != NULL && no == l.primeiro->proximo && no->musica.duracao == 120);
    verificar("CT-08", "busca do ID 9, inexistente", "NULL", buscar(&l, 9) == NULL);
    liberar_lista(&l);

    inicializar_lista(&l);
    inserir_no_final(&l, musica(1, "Lia", 60));
    inserir_no_final(&l, musica(2, "Caio", 60));
    inserir_no_final(&l, musica(3, "Lia", 60));
    no = buscar_por_artista(l.primeiro, "Lia");
    verificar("CT-09", "busca por artista com duas musicas", "IDs 1 e 3, depois NULL",
              no && no->musica.id == 1 &&
              (no = buscar_por_artista(no->proximo, "Lia")) != NULL && no->musica.id == 3 &&
              buscar_por_artista(no->proximo, "Lia") == NULL);
    liberar_lista(&l);

    l = criar(3);
    verificar("CT-10", "remocao da primeira de [1,2,3]", "[2,3]",
              remover(&l, 1, &m) == OK && m.id == 1 && ordem(&l, "23"));
    liberar_lista(&l);

    l = criar(3);
    verificar("CT-11", "remocao da intermediaria de [1,2,3]", "[1,3]",
              remover(&l, 2, &m) == OK && m.id == 2 && ordem(&l, "13"));
    liberar_lista(&l);

    l = criar(3);
    verificar("CT-12", "remocao da ultima de [1,2,3]", "[1,2], com NULL no fim",
              remover(&l, 3, &m) == OK && m.id == 3 && ordem(&l, "12"));
    liberar_lista(&l);

    l = criar(1);
    verificar("CT-13", "remocao da unica musica", "primeiro = NULL, quantidade = 0",
              remover(&l, 1, &m) == OK && l.primeiro == NULL && l.quantidade == 0);
    liberar_lista(&l);

    l = criar(3);
    verificar("CT-14", "remocao do ID 9, inexistente", "erro, lista inalterada",
              remover(&l, 9, &m) == ERRO_NAO_ENCONTRADA && ordem(&l, "123"));
    liberar_lista(&l);

    l = criar(4);
    verificar("CT-15", "quantidade com 4 musicas", "4", quantidade_musicas(&l) == 4);
    verificar("CT-16", "duracao de 60+120+180+240 s", "600 s", duracao_total(&l) == 600);
    liberar_lista(&l);
}

static void testar_inicializar_lista(void) {
    Lista l;
    l.primeiro = (No *)&l;
    l.quantidade = 42;
    inicializar_lista(&l);
    verificar("INI-01", "inicializar sobre lixo de memoria", "lista vazia", ordem(&l, ""));
    l = criar(3);
    liberar_lista(&l);
    verificar("INI-02", "liberar lista com 3 musicas", "lista vazia", ordem(&l, ""));
    liberar_lista(&l);
    verificar("INI-03", "liberar duas vezes", "lista vazia, sem falha", ordem(&l, ""));
    verificar("INI-04", "reutilizar lista liberada", "aceita insercao",
              inserir(&l, musica(1, "A", 1), 0) == OK && ordem(&l, "1"));
    liberar_lista(&l);
}

static void testar_inserir(void) {
    Lista l = criar(3);
    Musica m = musica(4, "Original", 60);
    int i, certo = 1;
    verificar("INS-01", "posicao negativa", "erro, lista inalterada",
              inserir(&l, m, -1) == ERRO_POSICAO && ordem(&l, "123"));
    verificar("INS-02", "posicao alem do final", "erro, lista inalterada",
              inserir(&l, m, 4) == ERRO_POSICAO && ordem(&l, "123"));
    verificar("INS-03", "ID repetido", "erro, lista inalterada",
              inserir(&l, musica(2, "B", 5), 0) == ERRO_ID_REPETIDO && ordem(&l, "123"));
    verificar("INS-04", "logo antes do ultimo", "[1,2,4,3]",
              inserir(&l, m, 2) == OK && ordem(&l, "1243"));
    strcpy(m.artista, "Alterado");
    verificar("INS-05", "alterar a variavel apos inserir", "o no guarda sua propria copia",
              strcmp(buscar(&l, 4)->musica.artista, "Original") == 0);
    remover(&l, 2, NULL);
    verificar("INS-06", "reutilizar ID removido", "aceita",
              inserir_no_final(&l, musica(2, "B", 5)) == OK && ordem(&l, "1432"));
    liberar_lista(&l);

    for (i = 1; i <= 5000; i++)
        if (inserir_no_inicio(&l, musica(i, "A", 1)) != OK) certo = 0;
    verificar("INS-07", "5000 insercoes sem capacidade previa", "5000 nos",
              certo && l.quantidade == 5000 && l.primeiro->musica.id == 5000);
    liberar_lista(&l);
}

static void testar_imprimir(void) {
    Lista l = criar(0);
    verificar("IMP-01", "imprimir lista vazia", "apenas NULL", impresso(&l, "Playlist: NULL\n"));
    liberar_lista(&l);
    l = criar(1);
    verificar("IMP-02", "imprimir uma musica", "cadeia e dados da musica",
              impresso(&l, "Playlist: [M1] -> NULL\n"
                           "  1. ID 1 | M1 | Artista | Album | 01:00\n"));
    liberar_lista(&l);
    l = criar(2);
    verificar("IMP-03", "imprimir duas musicas", "ordem da lista",
              impresso(&l, "Playlist: [M1] -> [M2] -> NULL\n"
                           "  1. ID 1 | M1 | Artista | Album | 01:00\n"
                           "  2. ID 2 | M2 | Artista | Album | 02:00\n"));
    liberar_lista(&l);
}

static void testar_buscar(void) {
    Lista l = criar(3), vazia = criar(0);
    verificar("BUS-01", "buscar em lista vazia", "NULL", buscar(&vazia, 1) == NULL);
    verificar("BUS-02", "buscar o primeiro", "primeiro no", buscar(&l, 1) == l.primeiro);
    verificar("BUS-03", "buscar o ultimo", "no com proximo = NULL",
              buscar(&l, 3) && buscar(&l, 3)->proximo == NULL);
    liberar_lista(&l);
}

static void testar_remover(void) {
    Lista l = criar(0);
    Musica m = musica(7, "A", 1);
    verificar("REM-01", "remover de lista vazia", "erro",
              remover(&l, 1, &m) == ERRO_NAO_ENCONTRADA && ordem(&l, ""));
    verificar("REM-02", "remocao que falha", "destino inalterado", m.id == 7);
    l = criar(3);
    verificar("REM-03", "destino NULL", "remove normalmente",
              remover(&l, 2, NULL) == OK && ordem(&l, "13"));
    verificar("REM-04", "remover o mesmo ID duas vezes", "erro na segunda",
              remover(&l, 2, NULL) == ERRO_NAO_ENCONTRADA && ordem(&l, "13"));
    remover(&l, 3, NULL);
    verificar("REM-05", "inserir no fim apos remover o ultimo", "[1,4]",
              inserir_no_final(&l, musica(4, "A", 1)) == OK && ordem(&l, "14"));
    liberar_lista(&l);
}

static void testar_playlist(void) {
    Lista l = criar(0);
    char t[TAM_DURACAO];
    verificar("PLA-01", "quantidade em lista vazia", "0", quantidade_musicas(&l) == 0);
    verificar("PLA-02", "duracao em lista vazia", "0 s", duracao_total(&l) == 0);
    verificar("PLA-03", "artista em lista vazia", "NULL", !buscar_por_artista(l.primeiro, "A"));
    inserir_no_inicio(&l, musica(1, "Érica LIMA", INT_MAX));
    verificar("PLA-04", "artista em outra caixa", "encontra",
              buscar_por_artista(l.primeiro, "érica lima") != NULL);
    verificar("PLA-05", "parte do nome do artista", "encontra",
              buscar_por_artista(l.primeiro, "lim") != NULL);
    verificar("PLA-06", "artista inexistente", "NULL", !buscar_por_artista(l.primeiro, "Caio"));
    verificar("PLA-07", "termo vazio", "NULL", !buscar_por_artista(l.primeiro, ""));
    inserir_no_final(&l, musica(2, "B", INT_MAX));
    verificar("PLA-08", "soma maior que um int", "sem estouro",
              duracao_total(&l) == 2LL * INT_MAX);
    remover(&l, 1, NULL);
    verificar("PLA-09", "totais apos remover", "quantidade e duracao atualizadas",
              quantidade_musicas(&l) == 1 && duracao_total(&l) == INT_MAX);
    formatar_duracao(65, t);
    verificar("DUR-01", "formatar 65 s", "01:05", strcmp(t, "01:05") == 0);
    formatar_duracao(3599, t);
    verificar("DUR-02", "formatar 3599 s", "59:59", strcmp(t, "59:59") == 0);
    formatar_duracao(3600, t);
    verificar("DUR-03", "formatar 3600 s", "1:00:00", strcmp(t, "1:00:00") == 0);
    liberar_lista(&l);
}

int main(void) {
    testar_obrigatorios();
    testar_inicializar_lista();
    testar_inserir();
    testar_imprimir();
    testar_buscar();
    testar_remover();
    testar_playlist();
    printf("\nUnitarios: %d testes, %d falhas\n", total, falhas);
    return falhas != 0;
}
