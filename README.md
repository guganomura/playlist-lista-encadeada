# Playlist com lista encadeada

Aplicação em C que gerencia uma playlist de músicas em uma lista dinâmica simplesmente encadeada,
implementada manualmente com `struct` e ponteiros. Usa apenas a biblioteca padrão do C.

- [Compilação e execução](#compilação-e-execução)
- [A estrutura de dados](#a-estrutura-de-dados)
- [Funcionalidades](#funcionalidades)
- [Exemplos de execução](#exemplos-de-execução)
- [Testes realizados](#testes-realizados)

## Compilação e execução

É necessário um compilador C99 (GCC ou Clang).

Com Make (macOS e Linux):

```bash
make        # compila
make run    # executa
make test   # roda todos os testes
make clean  # apaga os binários
```

Sem Make:

```bash
mkdir -p build
cc -std=c99 src/main.c src/musica.c src/lista.c src/playlist.c -o build/playlist
./build/playlist

cc -std=c99 tests/test_unitarios.c src/musica.c src/lista.c src/playlist.c -o build/testes
./build/testes
sh tests/test_sistema.sh
```

No Windows, com MinGW:

```bat
gcc -std=c99 src/main.c src/musica.c src/lista.c src/playlist.c -o playlist.exe
playlist.exe
```

## Organização

```text
src/musica.h, musica.c        dados de uma música
src/lista.h, lista.c          lista encadeada
src/playlist.h, playlist.c    funcionalidades construídas sobre a lista
src/main.c                    menu, leitura e validação da entrada
tests/test_unitarios.c        testes da lista e das funcionalidades
tests/test_sistema.sh         testes do programa completo
```

## A estrutura de dados

```c
typedef struct {
    int id;
    char titulo[TAM_TEXTO];
    char artista[TAM_TEXTO];
    char album[TAM_TEXTO];
    int duracao; /* em segundos */
} Musica;

typedef struct No {
    Musica musica;
    struct No *proximo;
} No;

typedef struct {
    No *primeiro;
    int quantidade;
} Lista;
```

Cada nó é criado com `malloc` no momento da inserção e devolvido com `free` na remoção. Por isso
não existe capacidade máxima definida previamente, e os nós podem ficar em posições quaisquer da
memória.

```text
Lista
+----------------+
| quantidade = 3 |
| primeiro       |---> [Musica 1 | *]---> [Musica 2 | *]---> [Musica 3 | *]---> NULL
+----------------+
```

### Como as referências mudam

Inserção no início:

```text
antes:    primeiro -> [A] -> [B] -> NULL

1. novo->proximo = primeiro         [N] -> [A]
2. primeiro = novo

depois:   primeiro -> [N] -> [A] -> [B] -> NULL
```

Inserção no meio ou no final (no final, `anterior->proximo` vale `NULL`):

```text
antes:    [A] -> [B] -> NULL        anterior = A

1. novo->proximo = anterior->proximo      [N] -> [B]
2. anterior->proximo = novo               [A] -> [N]

depois:   [A] -> [N] -> [B] -> NULL
```

Se os passos 1 e 2 fossem invertidos, a referência para B seria perdida.

Remoção do primeiro:

```text
antes:    primeiro -> [A] -> [B] -> NULL

1. primeiro = atual->proximo
2. free(atual)

depois:   primeiro -> [B] -> NULL
```

Remoção do meio ou do final:

```text
antes:    [A] -> [B] -> [C] -> NULL       anterior = A, atual = B

1. anterior->proximo = atual->proximo     [A] -> [C]
2. free(atual)

depois:   [A] -> [C] -> NULL
```

### Operações

| Operação | O que faz | Tempo |
|---|---|---|
| `inicializar_lista` | `primeiro = NULL` e `quantidade = 0` | O(1) |
| `inserir` | confere o ID, cria o nó e o encadeia na posição | O(n) |
| `imprimir` | percorre a lista até `NULL` | O(n) |
| `buscar` | procura o ID, nó por nó | O(n) |
| `remover` | acha o nó e o anterior, religa e libera | O(n) |

Encadear no início custa O(1), mas `inserir` continua O(n) porque antes verifica se o ID já existe.
`liberar_lista` é uma função auxiliar, necessária em C para devolver a memória.

## Funcionalidades

O número de cada opção do menu é o número do requisito no enunciado.

| Requisito e opção | Função | Tempo |
|---|---|---|
| 1. Inicializar playlist vazia | `liberar_lista` e `inicializar_lista` | O(n) |
| 2. Cadastrar nova música | `inserir`, perguntando o local | O(n) |
| 3. Inserir no início | `inserir_no_inicio` | O(n) |
| 4. Inserir no final | `inserir_no_final` | O(n) |
| 5. Inserir em uma posição | `inserir` | O(n) |
| 6. Exibir as músicas | `imprimir` | O(n) |
| 7. Buscar por ID | `buscar` | O(n) |
| 8. Buscar por artista | `buscar_por_artista` | O(n) |
| 9. Remover por ID | `remover` | O(n) |
| 10. Quantidade de músicas | `quantidade_musicas` | O(1) |
| 11. Duração total | `duracao_total` | O(n) |

### Decisões de projeto

- **ID** é um inteiro positivo, informado pelo usuário e único na playlist.
- **Posição** começa em 1 para o usuário e em 0 dentro da lista.
- **Busca por artista** aceita parte do nome e ignora maiúsculas e minúsculas, inclusive acentuadas
  em UTF-8. Os acentos contam: `joao` não encontra `João`.
- **Resultados da busca** não são copiados para outra estrutura. A própria lista é percorrida.
- **Duração** é digitada em `mm:ss` (de 00:01 a 999:59) e guardada em segundos.
- **Textos** têm até 99 bytes. O limite é por campo, não há limite de músicas além da memória.
- **Reinicializar** pede confirmação e libera todos os nós.
- **Memória insuficiente** é informada e deixa a playlist como estava.

## Exemplos de execução

As músicas são fictícias. O menu reaparece após cada operação e só é mostrado na primeira vez.

### Exemplo 1: inserções, exibição, quantidade e duração

```text
=== PLAYLIST ===
 1. Inicializar playlist vazia
 2. Cadastrar nova musica
 3. Inserir no inicio
 4. Inserir no final
 5. Inserir em uma posicao
 6. Exibir playlist
 7. Buscar por ID
 8. Buscar por artista
 9. Remover por ID
10. Quantidade de musicas
11. Duracao total
 0. Sair
Opcao: 1
Playlist vazia inicializada.

Opcao: 4
ID: 1
Titulo: Manhã de Sol
Artista: Lia Campos
Album: Horizonte
Duracao (mm:ss): 03:20
Musica inserida na posicao 1.

Opcao: 3
ID: 2
Titulo: Estrada
Artista: Caio Reis
Album: Viagem
Duracao (mm:ss): 04:05
Musica inserida na posicao 1.

Opcao: 5
ID: 3
Titulo: Maré Alta
Artista: Lia Campos
Album: Horizonte
Duracao (mm:ss): 02:50
Posicao (1 = inicio): 2
Musica inserida na posicao 2.

Opcao: 6
Playlist: [Estrada] -> [Maré Alta] -> [Manhã de Sol] -> NULL
  1. ID 2 | Estrada | Caio Reis | Viagem | 04:05
  2. ID 3 | Maré Alta | Lia Campos | Horizonte | 02:50
  3. ID 1 | Manhã de Sol | Lia Campos | Horizonte | 03:20

Opcao: 10
Quantidade de musicas: 3

Opcao: 11
Duracao total: 10:15

Opcao: 0
Programa encerrado.
```

### Exemplo 2: cadastro, buscas, remoções e reinicialização

```text
Opcao: 4
ID: 1
Titulo: Aurora
Artista: Nina Luz
Album: Céu
Duracao (mm:ss): 03:00
Musica inserida na posicao 1.

Opcao: 4
ID: 2
Titulo: Brisa
Artista: Caio Reis
Album: Vento
Duracao (mm:ss): 02:30
Musica inserida na posicao 2.

Opcao: 2
ID: 3
Titulo: Cais
Artista: Nina Luz
Album: Porto
Duracao (mm:ss): 04:15
Onde inserir? 1-inicio 2-final 3-posicao: 2
Musica inserida na posicao 3.

Opcao: 6
Playlist: [Aurora] -> [Brisa] -> [Cais] -> NULL
  1. ID 1 | Aurora | Nina Luz | Céu | 03:00
  2. ID 2 | Brisa | Caio Reis | Vento | 02:30
  3. ID 3 | Cais | Nina Luz | Porto | 04:15

Opcao: 7
ID procurado: 2
Encontrada: ID 2 | Brisa | Caio Reis | Vento | 02:30

Opcao: 7
ID procurado: 9
Nenhuma musica com ID 9.

Opcao: 8
Artista procurado: nina
  ID 1 | Aurora | Nina Luz | Céu | 03:00
  ID 3 | Cais | Nina Luz | Porto | 04:15
2 musica(s) encontrada(s).

Opcao: 9
ID a remover: 2
Removida: ID 2 | Brisa | Caio Reis | Vento | 02:30

Opcao: 6
Playlist: [Aurora] -> [Cais] -> NULL
  1. ID 1 | Aurora | Nina Luz | Céu | 03:00
  2. ID 3 | Cais | Nina Luz | Porto | 04:15

Opcao: 9
ID a remover: 8
Nenhuma musica com ID 8.

Opcao: 1
Apagar a playlist atual? (s/n): s
Playlist vazia inicializada.

Opcao: 6
Playlist: NULL

Opcao: 0
Programa encerrado.
```

## Testes realizados

São **91 testes automatizados**: 50 unitários e 41 de sistema. Todos passam.
Para repetir:

```bash
make test
```

Cada teste imprime `[OK]` ou `[FALHA]` com seu identificador, e o comando termina com erro se algum
falhar.

```text
Unitarios: 50 testes, 0 falhas
Sistema: 41 testes, 0 falhas
```

### Casos de teste obrigatórios

Cada caso é testado duas vezes: diretamente nas funções e pelo menu do programa. Nas inserções e
remoções, os testes unitários conferem a ordem dos nós, a quantidade e o `NULL` final.

| Nº | Caso do enunciado | Resultado esperado | Unitário | Pelo menu | Situação |
|:-:|---|---|---|---|:-:|
| 1 | Criação de uma playlist vazia | `primeiro = NULL`, quantidade 0 | CT-01 | SIS-01 | Passou |
| 2 | Inserção da primeira música | `[1] -> NULL` | CT-02 | SIS-02 | Passou |
| 3 | Inserção de várias músicas | `[1,2,3,4,5]` | CT-03 | SIS-03 | Passou |
| 4 | Inserção no início | `[1,2,3]` vira `[4,1,2,3]` | CT-04 | SIS-04 | Passou |
| 5 | Inserção no final | `[1,2,3]` vira `[1,2,3,4]` | CT-05 | SIS-05 | Passou |
| 6 | Inserção no meio | `[1,2,3,4]` vira `[1,2,5,3,4]` | CT-06 | SIS-06 | Passou |
| 7 | Busca de música existente | retorna o nó do ID 2 | CT-07 | SIS-07 | Passou |
| 8 | Busca de música inexistente | retorna `NULL` | CT-08 | SIS-08 | Passou |
| 9 | Busca por artista | retorna os IDs 1 e 3 | CT-09 | SIS-09 | Passou |
| 10 | Remoção da primeira | `[1,2,3]` vira `[2,3]` | CT-10 | SIS-10 | Passou |
| 11 | Remoção de intermediária | `[1,2,3]` vira `[1,3]` | CT-11 | SIS-11 | Passou |
| 12 | Remoção da última | `[1,2,3]` vira `[1,2]` | CT-12 | SIS-12 | Passou |
| 13 | Remoção da única | lista vazia | CT-13 | SIS-13 | Passou |
| 14 | Remoção de inexistente | erro, lista inalterada | CT-14 | SIS-14 | Passou |
| 15 | Cálculo da quantidade | 4 | CT-15 | SIS-15 | Passou |
| 16 | Cálculo da duração total | 60+120+180+240 = 600 s | CT-16 | SIS-16 | Passou |

### Testes adicionais da estrutura

| ID | Cenário | Resultado esperado |
|---|---|---|
| INI-01 | inicializar sobre lixo de memoria | `lista vazia` |
| INI-02 | liberar lista com 3 musicas | `lista vazia` |
| INI-03 | liberar duas vezes | `lista vazia, sem falha` |
| INI-04 | reutilizar lista liberada | `aceita insercao` |
| INS-01 | posicao negativa | `erro, lista inalterada` |
| INS-02 | posicao alem do final | `erro, lista inalterada` |
| INS-03 | ID repetido | `erro, lista inalterada` |
| INS-04 | logo antes do ultimo | `[1,2,4,3]` |
| INS-05 | alterar a variavel apos inserir | `o no guarda sua propria copia` |
| INS-06 | reutilizar ID removido | `aceita` |
| INS-07 | 5000 insercoes sem capacidade previa | `5000 nos` |
| IMP-01 | imprimir lista vazia | `apenas NULL` |
| IMP-02 | imprimir uma musica | `cadeia e dados da musica` |
| IMP-03 | imprimir duas musicas | `ordem da lista` |
| BUS-01 | buscar em lista vazia | `NULL` |
| BUS-02 | buscar o primeiro | `primeiro no` |
| BUS-03 | buscar o ultimo | `no com proximo = NULL` |
| REM-01 | remover de lista vazia | `erro` |
| REM-02 | remocao que falha | `destino inalterado` |
| REM-03 | destino NULL | `remove normalmente` |
| REM-04 | remover o mesmo ID duas vezes | `erro na segunda` |
| REM-05 | inserir no fim apos remover o ultimo | `[1,4]` |

### Testes adicionais das funcionalidades

| ID | Cenário | Resultado esperado |
|---|---|---|
| PLA-01 | quantidade em lista vazia | `0` |
| PLA-02 | duracao em lista vazia | `0 s` |
| PLA-03 | artista em lista vazia | `NULL` |
| PLA-04 | artista em outra caixa | `encontra` |
| PLA-05 | parte do nome do artista | `encontra` |
| PLA-06 | artista inexistente | `NULL` |
| PLA-07 | termo vazio | `NULL` |
| PLA-08 | soma maior que um int | `sem estouro` |
| PLA-09 | totais apos remover | `quantidade e duracao atualizadas` |
| DUR-01 | formatar 65 s | `01:05` |
| DUR-02 | formatar 3599 s | `59:59` |
| DUR-03 | formatar 3600 s | `1:00:00` |

### Testes do sistema

Enviam entradas ao executável, como um usuário faria, e conferem a saída.
A = ID 1 (01:00), B = ID 2 (02:00), C = ID 3 (03:30) e D = ID 4 (04:00).

| ID | Cenário | Resultado esperado |
|---|---|---|
| SIS-01 | criar playlist vazia | `lista vazia` |
| SIS-02 | inserir a primeira | `[A]` |
| SIS-03 | inserir varias | `[A,B,C]` |
| SIS-04 | inserir no inicio | `[D,A,B,C]` |
| SIS-05 | inserir no final | `[A,B,C,D]` |
| SIS-06 | inserir na posicao 2 | `[A,D,B,C]` |
| SIS-07 | buscar ID existente | `mostra a musica` |
| SIS-08 | buscar ID inexistente | `avisa` |
| SIS-09 | buscar por artista | `2 resultados` |
| SIS-10 | remover a primeira | `[B,C]` |
| SIS-11 | remover a intermediaria | `[A,C]` |
| SIS-12 | remover a ultima | `[A,B]` |
| SIS-13 | remover a unica | `lista vazia` |
| SIS-14 | remover inexistente | `avisa` |
| SIS-15 | contar musicas | `3` |
| SIS-16 | somar duracao | `06:30` |
| SIS-17 | cadastrar no inicio | `[D,A,B,C]` |
| SIS-18 | cadastrar no final | `[A,B,C,D]` |
| SIS-19 | cadastrar em posicao | `[A,B,D,C]` |
| SIS-20 | exibir os cinco campos | `linha completa` |
| SIS-21 | reinicializar confirmando | `lista vazia` |
| SIS-22 | cancelar reinicializacao | `lista mantida` |
| SIS-23 | confirmacao invalida | `pede de novo` |
| SIS-24 | ID repetido | `pede outro` |
| SIS-25 | ID zero | `pede de novo` |
| SIS-26 | ID com texto | `pede de novo` |
| SIS-27 | ID grande demais | `pede de novo` |
| SIS-28 | titulo vazio | `pede de novo` |
| SIS-29 | segundos acima de 59 | `pede de novo` |
| SIS-30 | duracao zero | `pede de novo` |
| SIS-31 | duracao sem dois-pontos | `pede de novo` |
| SIS-32 | posicao fora do intervalo | `pede de novo` |
| SIS-33 | posicao em lista vazia | `aceita 1` |
| SIS-34 | opcao invalida | `pede de novo` |
| SIS-35 | artista inexistente | `0 resultados` |
| SIS-36 | artista em minusculas | `2 resultados` |
| SIS-37 | nomes com espaco e acento | `preserva o texto` |
| SIS-38 | quantidade em lista vazia | `0` |
| SIS-39 | duracao em lista vazia | `00:00` |
| SIS-40 | total acima de uma hora | `1:10:00` |
| SIS-41 | fim inesperado da entrada | `encerra sem travar` |
