#ifndef TRATA_GEO_H
#define TRATA_GEO_H

#include "hash_table.h"
#include "leitor_arquivos.h"

/*
 * Modulo de processamento do arquivo .geo.
 *
 * O modulo interpreta os comandos que descrevem as quadras da cidade,
 * armazena cada quadra em uma tabela hash indexada pelo CEP e gera o primeiro
 * arquivo SVG do projeto. Esse SVG representa o estado inicial da cidade,
 * antes da execucao dos comandos do arquivo .qry.
 *
 * Comandos processados:
 * - cq sw cfill cstrk: define o estilo das quadras criadas a partir dele;
 * - q cep x y w h: cria uma quadra retangular.
 */

/**
 * @brief Processa um arquivo .geo e gera seu SVG inicial.
 *
 * O nome do SVG e obtido a partir do nome base do arquivo .geo. Por exemplo,
 * `cidade.geo` produz `cidade.svg` dentro de `diretorioSaida`.
 *
 * A tabela retornada passa a ser responsabilidade do chamador e deve ser
 * liberada com `desaloca_geo`.
 *
 * @param dadosGeo Arquivo .geo previamente carregado pelo leitor de arquivos
 * @param diretorioSaida Diretorio onde o SVG inicial sera criado
 * @return Tabela hash contendo as quadras ou NULL em caso de erro
 */
HashTable tratar_arquivo_geo(DadosDoArquivo dadosGeo,
                             const char *diretorioSaida);

/**
 * @brief Libera as memorias alocadas pelo processamento do arquivo .geo.
 *
 * A funcao libera a tabela hash, suas chaves e todas as quadras armazenadas.
 *
 * @param quadras Tabela retornada por tratar_arquivo_geo
 */
void desaloca_geo(HashTable quadras);

#endif
