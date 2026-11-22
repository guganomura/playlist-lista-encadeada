#!/bin/sh
# Executa o programa com entradas prontas e confere a saida de cada cenario.
# SIS-01 a SIS-16 repetem, pelo menu, os 16 casos obrigatorios.
# Uso: sh tests/test_sistema.sh [executavel]

programa=${1:-build/playlist}
total=0
falhas=0

# testar ID CENARIO ESPERADO ENTRADA TRECHO_DA_SAIDA
testar() {
    total=$((total + 1))
    if saida=$(printf "$4" | "$programa" 2>&1) &&
        printf '%s\n' "$saida" | grep -Fq -- "$5"; then
        echo "[OK] $1 $2 -> $3"
    else
        falhas=$((falhas + 1))
        echo "[FALHA] $1 $2 -> $3"
    fi
}

# Dados de cada musica: ID, titulo, artista, album e duracao.
A='1\nA\nLia\nMar\n01:00\n'
B='2\nB\nCaio\nSol\n02:00\n'
C='3\nC\nLia\nMar\n03:30\n'
D='4\nD\nNina\nLua\n04:00\n'
ABC="4\n${A}4\n${B}4\n${C}"

testar SIS-01 "criar playlist vazia" "lista vazia" '1\n6\n0\n' "Playlist: NULL"
testar SIS-02 "inserir a primeira" "[A]" "4\n${A}6\n0\n" "Playlist: [A] -> NULL"
testar SIS-03 "inserir varias" "[A,B,C]" "${ABC}6\n0\n" "Playlist: [A] -> [B] -> [C] -> NULL"
testar SIS-04 "inserir no inicio" "[D,A,B,C]" "${ABC}3\n${D}6\n0\n" "Playlist: [D] -> [A] -> [B] -> [C] -> NULL"
testar SIS-05 "inserir no final" "[A,B,C,D]" "${ABC}4\n${D}6\n0\n" "Playlist: [A] -> [B] -> [C] -> [D] -> NULL"
testar SIS-06 "inserir na posicao 2" "[A,D,B,C]" "${ABC}5\n${D}2\n6\n0\n" "Playlist: [A] -> [D] -> [B] -> [C] -> NULL"
testar SIS-07 "buscar ID existente" "mostra a musica" "${ABC}7\n2\n0\n" "Encontrada: ID 2 | B | Caio | Sol | 02:00"
testar SIS-08 "buscar ID inexistente" "avisa" "${ABC}7\n9\n0\n" "Nenhuma musica com ID 9"
testar SIS-09 "buscar por artista" "2 resultados" "${ABC}8\nLia\n0\n" "2 musica(s) encontrada(s)"
testar SIS-10 "remover a primeira" "[B,C]" "${ABC}9\n1\n6\n0\n" "Playlist: [B] -> [C] -> NULL"
testar SIS-11 "remover a intermediaria" "[A,C]" "${ABC}9\n2\n6\n0\n" "Playlist: [A] -> [C] -> NULL"
testar SIS-12 "remover a ultima" "[A,B]" "${ABC}9\n3\n6\n0\n" "Playlist: [A] -> [B] -> NULL"
testar SIS-13 "remover a unica" "lista vazia" "4\n${A}9\n1\n6\n0\n" "Playlist: NULL"
testar SIS-14 "remover inexistente" "avisa" "${ABC}9\n9\n0\n" "Nenhuma musica com ID 9"
testar SIS-15 "contar musicas" "3" "${ABC}10\n0\n" "Quantidade de musicas: 3"
testar SIS-16 "somar duracao" "06:30" "${ABC}11\n0\n" "Duracao total: 06:30"

testar SIS-17 "cadastrar no inicio" "[D,A,B,C]" "${ABC}2\n${D}1\n6\n0\n" "Playlist: [D] -> [A] -> [B] -> [C] -> NULL"
testar SIS-18 "cadastrar no final" "[A,B,C,D]" "${ABC}2\n${D}2\n6\n0\n" "Playlist: [A] -> [B] -> [C] -> [D] -> NULL"
testar SIS-19 "cadastrar em posicao" "[A,B,D,C]" "${ABC}2\n${D}3\n3\n6\n0\n" "Playlist: [A] -> [B] -> [D] -> [C] -> NULL"
testar SIS-20 "exibir os cinco campos" "linha completa" "${ABC}6\n0\n" "3. ID 3 | C | Lia | Mar | 03:30"
testar SIS-21 "reinicializar confirmando" "lista vazia" "${ABC}1\ns\n6\n0\n" "Playlist: NULL"
testar SIS-22 "cancelar reinicializacao" "lista mantida" "${ABC}1\nn\n6\n0\n" "Playlist: [A] -> [B] -> [C] -> NULL"
testar SIS-23 "confirmacao invalida" "pede de novo" "${ABC}1\nx\nn\n0\n" "Responda com s ou n"
testar SIS-24 "ID repetido" "pede outro" "${ABC}4\n2\n${D}0\n" "Ja existe uma musica com ID 2"
testar SIS-25 "ID zero" "pede de novo" "4\n0\n${A}0\n" "Digite um numero inteiro"
testar SIS-26 "ID com texto" "pede de novo" "4\nabc\n${A}0\n" "Digite um numero inteiro"
testar SIS-27 "ID grande demais" "pede de novo" "4\n99999999999999999999\n${A}0\n" "Digite um numero inteiro"
testar SIS-28 "titulo vazio" "pede de novo" '4\n1\n  \nA\nLia\nMar\n01:00\n0\n' "O campo nao pode ficar vazio"
testar SIS-29 "segundos acima de 59" "pede de novo" '4\n1\nA\nLia\nMar\n01:60\n01:00\n0\n' "Use o formato mm:ss"
testar SIS-30 "duracao zero" "pede de novo" '4\n1\nA\nLia\nMar\n00:00\n01:00\n0\n' "Use o formato mm:ss"
testar SIS-31 "duracao sem dois-pontos" "pede de novo" '4\n1\nA\nLia\nMar\n90\n01:30\n0\n' "Use o formato mm:ss"
testar SIS-32 "posicao fora do intervalo" "pede de novo" "${ABC}5\n${D}5\n4\n0\n" "entre 1 e 4"
testar SIS-33 "posicao em lista vazia" "aceita 1" "5\n${A}1\n6\n0\n" "Playlist: [A] -> NULL"
testar SIS-34 "opcao invalida" "pede de novo" '12\n0\n' "entre 0 e 11"
testar SIS-35 "artista inexistente" "0 resultados" "${ABC}8\nZeca\n0\n" "0 musica(s) encontrada(s)"
testar SIS-36 "artista em minusculas" "2 resultados" "${ABC}8\nlia\n0\n" "2 musica(s) encontrada(s)"
testar SIS-37 "nomes com espaco e acento" "preserva o texto" '4\n1\nCéu de Março\nJoão Sá\nManhã\n02:05\n6\n0\n' "ID 1 | Céu de Março | João Sá | Manhã | 02:05"
testar SIS-38 "quantidade em lista vazia" "0" '10\n0\n' "Quantidade de musicas: 0"
testar SIS-39 "duracao em lista vazia" "00:00" '11\n0\n' "Duracao total: 00:00"
testar SIS-40 "total acima de uma hora" "1:10:00" '4\n1\nA\nX\nY\n40:00\n4\n2\nB\nX\nY\n30:00\n11\n0\n' "Duracao total: 1:10:00"
testar SIS-41 "fim inesperado da entrada" "encerra sem travar" '4\n1\nA\n' "Programa encerrado"

echo
echo "Sistema: $total testes, $falhas falhas"
[ "$falhas" -eq 0 ]
