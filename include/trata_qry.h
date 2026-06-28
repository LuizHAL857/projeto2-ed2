#ifndef TRATA_QRY_H
#define TRATA_QRY_H

#include <stdbool.h>

#include "hash_table.h"
#include "leitor_arquivos.h"
#include "trata_via.h"

/*
 * Modulo de processamento do arquivo .qry.
 *
 * O modulo executa as consultas sobre as quadras e o sistema viario da
 * cidade. Durante o processamento, mantem os registradores R0 a R10 e gera
 * as respostas textual e svg previstas pelo projeto.
 *
 */

/**
 * @brief Tipo opaco para o estado produzido pelo processamento do .qry.
 *
 * A estrutura mantem os registradores e outros dados temporarios das
 * consultas. Ela nao e proprietaria das quadras, de DadosVia ou dos arquivos
 * de entrada recebidos por tratar_arquivo_qry.
 */
typedef void *DadosQry;

/**
 * @brief Processa um arquivo .qry e gera seus arquivos SVG e TXT.
 *
 * Os comandos sao executados na ordem em que aparecem. Alteracoes feitas no
 * grafo, como as dos comandos mvm e exp, permanecem em dadosVia e influenciam
 * as consultas seguintes.
 *
 * Os nomes das saidas combinam os nomes-base do GEO e do QRY. Por exemplo,
 *
 * @param dadosQryArquivo Arquivo .qry carregado pelo leitor de arquivos
 * @param dadosGeoArquivo Arquivo .geo usado para determinar o nome das saidas
 * @param quadras Tabela de quadras indexada por CEP
 * @param dadosVia Grafo e indice de vertices construidos a partir do .via
 * @param diretorioSaida Diretorio onde o SVG e o TXT serao criados
 * @return Estado das consultas ou NULL em caso de erro
 */
DadosQry tratar_arquivo_qry(DadosDoArquivo dadosQryArquivo,
                            DadosDoArquivo dadosGeoArquivo,
                            HashTable quadras,
                            DadosVia dadosVia,
                            const char *diretorioSaida);

/**
 * @brief Consulta a coordenada armazenada em um registrador.
 *
 * @param dadosQry Estado retornado por tratar_arquivo_qry
 * @param indiceRegistrador Indice entre 0 e 10, correspondente a R0 ate R10
 * @param x Ponteiro que recebera a coordenada x
 * @param y Ponteiro que recebera a coordenada y
 * @return true se o registrador estiver definido, false caso contrario
 */
bool obter_registrador_qry(DadosQry dadosQry, int indiceRegistrador,
                           float *x, float *y);

/**
 * @brief Libera a memoria interna mantida pelo processamento do .qry.
 *
 * As quadras, DadosVia e DadosDoArquivo recebidos no processamento continuam
 * sob responsabilidade do chamador e nao sao liberados por esta funcao.
 *
 * @param dadosQry Estado retornado por tratar_arquivo_qry
 */
void desaloca_qry(DadosQry dadosQry);

#endif
