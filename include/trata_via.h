#ifndef TRATA_VIA_H
#define TRATA_VIA_H

#include "grafo_direcionado.h"
#include "leitor_arquivos.h"

/*
 * Modulo de processamento do arquivo .via.
 *
 * O arquivo .via descreve o sistema viario da cidade. Este modulo interpreta
 * os comandos de vertices e arestas, constroi o grafo direcionado e mantem um
 * indice auxiliar que associa os identificadores textuais do arquivo aos
 * vertices criados no grafo.
 *
 */

/**
 * @brief Tipo opaco para o resultado do processamento do .via.
 */
typedef void* DadosVia;

/**
 * @brief Processa um arquivo .via e constroi o grafo viario.
 *
 * @param dadosViaArquivo Arquivo .via carregado pelo leitor de arquivos
 * @return Estrutura contendo o grafo e o indice de vertices, ou NULL em erro
 */
DadosVia tratar_arquivo_via(DadosDoArquivo dadosViaArquivo);

/**
 * @brief Libera as memorias alocadas pelo processamento do arquivo .via.
 *
 * @param dadosVia Estrutura retornada por tratar_arquivo_via
 */
void desaloca_via(DadosVia dadosVia);

/**
 * @brief Retorna o grafo construido a partir do arquivo .via.
 *
 * O grafo pertence a DadosVia e nao deve ser liberado separadamente.
 *
 * @param dadosVia Estrutura retornada por tratar_arquivo_via
 * @return Grafo direcionado ou NULL em caso de parametro invalido
 */
Grafo obter_grafo_via(DadosVia dadosVia);

/**
 * @brief Busca um vertice pelo identificador textual usado no arquivo .via.
 *
 * @param dadosVia Estrutura retornada por tratar_arquivo_via
 * @param idVertice Identificador textual do vertice
 * @return Vertice encontrado ou NULL se nao existir
 */
Vertice obter_vertice_via(DadosVia dadosVia, const char *idVertice);

#endif
