#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdbool.h>

/*
 * Módulo de hash table com chave textual.
 *
 * 
 * o módulo tem o intuito de armazenar as quadras a cidade para otimizar o tempo de busca por uma quadra
 * com base no cep, visto que a cidade sera numerosa em quadras, e a busca comum poderia ser excessivamente demorada 
 * caso não fosse usada tal estrutura
 */

/**
 * @brief Tipo opaco para hash table
 */
typedef void* HashTable;

/**
 * @brief Tipo de callback usado para liberar os valores armazenados.
 *
 * @param valor Conteúdo associado a uma chave da tabela
 */
typedef void (*HashDestruirValor)(void *valor);

/**
 * @brief Tipo de callback usado para percorrer os pares da tabela.
 *
 * @param chave Chave textual armazenada
 * @param valor Conteúdo associado à chave
 * @param extra Ponteiro auxiliar fornecido pelo chamador
 */
typedef void (*HashVisitaPar)(const char *chave, void *valor, void *extra);

/**
 * @brief Cria uma tabela hash vazia.
 *
 * @param capacidadeInicial Quantidade inicial de posições da tabela
 * @return Ponteiro para a tabela criada ou NULL em caso de erro
 */
HashTable criaHashTable(int capacidadeInicial);

/**
 * @brief Libera a tabela hash e seus elementos.
 *
 *
 * @param tabela Tabela a ser liberada
 * @param destruirValor Função opcional para desalocar os valores
 */
void liberaHashTable(HashTable tabela, HashDestruirValor destruirValor);

/**
 * @brief Insere ou atualiza um valor associado a uma chave.
 *
 *
 * @param tabela Tabela onde o par será armazenado
 * @param chave Chave textual do elemento
 * @param valor Conteúdo associado à chave
 * 
 * @return true em caso de sucesso, false em caso de erro
 */
bool insereHashTable(HashTable tabela, const char *chave, void *valor);

/**
 * @brief Busca o valor associado a uma chave.
 *
 * @param tabela Tabela a ser consultada
 * @param chave Chave textual procurada
 * 
 * @return Valor associado à chave ou NULL se ela não existir
 */
void* buscaHashTable(HashTable tabela, const char *chave);

/**
 * @brief Verifica se uma chave está presente na tabela.
 *
 * @param tabela Tabela a ser consultada
 * @param chave Chave textual procurada
 * 
 * @return true se a chave existir, false caso contrário
 */
bool chaveExisteHashTable(HashTable tabela, const char *chave);

/**
 * @brief Remove uma chave da tabela e retorna o valor associado.
 *
 *
 * @param tabela Tabela a ser alterada
 * @param chave Chave textual a ser removida
 * 
 * @return Valor removido ou NULL se a chave não existir
 */
void* removeHashTable(HashTable tabela, const char *chave);

/**
 * @brief Retorna a quantidade de pares armazenados na tabela.
 *
 * @param tabela Tabela consultada
 * @return Número de elementos armazenados
 */
int getTamanhoHashTable(HashTable tabela);

/**
 * @brief Percorre todos os pares armazenados na tabela.
 *
 *
 * @param tabela Tabela a ser percorrida
 * @param visita Callback executado para cada par
 * @param extra Ponteiro auxiliar repassado ao callback
 */
void percorreHashTable(HashTable tabela, HashVisitaPar visita, void *extra);

#endif
