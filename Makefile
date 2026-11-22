CC ?= cc
CFLAGS ?= -std=c99 -Wall -Wextra -pedantic

MODULOS = src/musica.c src/lista.c src/playlist.c
CABECALHOS = src/musica.h src/lista.h src/playlist.h

all: build/playlist

build/playlist: src/main.c $(MODULOS) $(CABECALHOS)
	@mkdir -p build
	$(CC) $(CFLAGS) src/main.c $(MODULOS) -o $@

build/testes: tests/test_unitarios.c $(MODULOS) $(CABECALHOS)
	@mkdir -p build
	$(CC) $(CFLAGS) tests/test_unitarios.c $(MODULOS) -o $@

run: build/playlist
	./build/playlist

test: build/testes build/playlist
	./build/testes
	sh tests/test_sistema.sh

clean:
	rm -rf build

.PHONY: all run test clean
