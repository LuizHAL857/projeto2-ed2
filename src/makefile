PROJ_NAME = ted

CC = gcc
CPPFLAGS = -I../include -I../unity
CFLAGS = -ggdb -O0 -std=c99 -fstack-protector-all -Wall -Wextra \
	-Werror=implicit-function-declaration
LDFLAGS = -O0
LDLIBS =

UNITY_OBJ = unity.o

MODULOS_OBJ = \
	lista.o \
	trata_argumentos.o \
	leitor_arquivos.o \
	hash_table.o \
	quadra.o \
	grafo_direcionado.o \
	trata_geo.o \
	trata_via.o

TESTES = \
	t_lista \
	t_trata_argumentos \
	t_leitor_arquivos \
	t_hash_table \
	t_quadra \
	t_grafo_direcionado \
	t_trata_geo \
	t_trata_via

.PHONY: all tests tstall clean \
	test_lista test_trata_argumentos test_leitor_arquivos \
	test_hash_table test_quadra test_grafo_direcionado test_trata_geo \
	test_trata_via

all: ted

ifneq ($(wildcard main.c),)
ted: main.o $(MODULOS_OBJ)
	$(CC) $(LDFLAGS) -o $(PROJ_NAME) main.o $(MODULOS_OBJ) $(LDLIBS)

main.o: main.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@
else
.PHONY: ted
ted:
	@printf '%s\n' \
		'Erro: src/main.c ainda nao existe; nao foi possivel gerar ted.'
	@false
endif

#
# Objetos dos modulos
#
lista.o: lista.c ../include/lista.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

trata_argumentos.o: trata_argumentos.c ../include/trata_argumentos.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

leitor_arquivos.o: leitor_arquivos.c ../include/leitor_arquivos.h \
	../include/lista.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

hash_table.o: hash_table.c ../include/hash_table.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

quadra.o: quadra.c ../include/quadra.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

grafo_direcionado.o: grafo_direcionado.c \
	../include/grafo_direcionado.h ../include/lista.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

trata_geo.o: trata_geo.c ../include/trata_geo.h \
	../include/leitor_arquivos.h ../include/hash_table.h \
	../include/quadra.h ../include/lista.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

trata_via.o: trata_via.c ../include/trata_via.h \
	../include/leitor_arquivos.h ../include/grafo_direcionado.h \
	../include/hash_table.h ../include/lista.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(UNITY_OBJ): ../unity/unity.c ../unity/unity.h \
	../unity/unity_internals.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c ../unity/unity.c -o $@

#
# Compilacao dos testes unitarios
#
t_lista: ../test/test_lista.c lista.o $(UNITY_OBJ)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

t_trata_argumentos: ../test/test_trata_argumentos.c \
	trata_argumentos.o $(UNITY_OBJ)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

t_leitor_arquivos: ../test/test_leitor_arquivos.c \
	leitor_arquivos.o lista.o $(UNITY_OBJ)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

t_hash_table: ../test/test_hash_table.c hash_table.o $(UNITY_OBJ)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

t_quadra: ../test/test_quadra.c quadra.o $(UNITY_OBJ)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

t_grafo_direcionado: ../test/test_grafo_direcionado.c \
	grafo_direcionado.o lista.o $(UNITY_OBJ)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

t_trata_geo: ../test/test_trata_geo.c trata_geo.o \
	leitor_arquivos.o lista.o hash_table.o quadra.o $(UNITY_OBJ)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

t_trata_via: ../test/test_trata_via.c trata_via.o \
	leitor_arquivos.o lista.o hash_table.o grafo_direcionado.o $(UNITY_OBJ)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

#
# Execucao individual dos testes
#
test_lista: t_lista
	./t_lista

test_trata_argumentos: t_trata_argumentos
	./t_trata_argumentos

test_leitor_arquivos: t_leitor_arquivos
	./t_leitor_arquivos

test_hash_table: t_hash_table
	./t_hash_table

test_quadra: t_quadra
	./t_quadra

test_grafo_direcionado: t_grafo_direcionado
	./t_grafo_direcionado

test_trata_geo: t_trata_geo
	./t_trata_geo

test_trata_via: t_trata_via
	./t_trata_via

tests: $(TESTES)

tstall: tests
	./t_lista
	./t_trata_argumentos
	./t_leitor_arquivos
	./t_hash_table
	./t_quadra
	./t_grafo_direcionado
	./t_trata_geo
	./t_trata_via

clean:
	rm -f *.o $(PROJ_NAME) $(TESTES)
