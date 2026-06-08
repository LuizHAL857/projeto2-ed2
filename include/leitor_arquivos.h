#ifndef LEITOR_ARQUIVOS_H
#define LEITOR_ARQUIVOS_H

/*
 * Módulo de leitura de arquivos texto para memória.
 *
 * O módulo encapsula os dados de um arquivo lido , preservando seu
 * caminho, nome e uma lista com as linhas do conteúdo. Ele também centraliza a
 * criação e a destruição dessas estruturas auxiliares.
 */

#include <stdbool.h>
#include <stdio.h>
#include "../include/lista.h"

/* Tipo opaco para os dados carregados de um arquivo. */
typedef void* DadosDoArquivo;
 
 /**
  * Cria uma nova instância de DadosDoArquivo e lê o conteúdo do arquivo.
  *
  * @param caminhoArquivo Caminho completo para o arquivo.
  * @return Instância de DadosDoArquivo ou NULL em caso de erro.
  */
DadosDoArquivo criar_dados_arquivo(char *caminhoArquivo);
 
 /**
  * Destroi uma instância de DadosDoArquivo e libera toda a memória associada.
  *
  * @param dadosArquivo A instância a ser destruída.
  */
void destruir_dados_arquivo(DadosDoArquivo dadosArquivo);
 
 /**
  * Obtém o caminho completo do arquivo.
  *
  * @param dadosArquivo Instância de DadosDoArquivo.
  * @return Ponteiro para string com o caminho do arquivo.
  */
char *obter_caminho_arquivo(DadosDoArquivo dadosArquivo);
 
 /**
  * Obtém o nome do arquivo (sem o caminho).
  *
  * @param dadosArquivo Instância de DadosDoArquivo.
  * @return Ponteiro para string com o nome do arquivo.
  */
char *obter_nome_arquivo(DadosDoArquivo dadosArquivo);
 
 /**
  * Obtém a fila com as linhas do arquivo.
  *
  * @param dadosArquivo Instância de DadosDoArquivo.
  * @return Lista contendo as linhas do arquivo.
  */
Lista obter_lista_linhas(DadosDoArquivo dadosArquivo);

/**
 * @brief Verifica se um texto termina com um determinado sufixo.
 *
 *
 * @param texto Texto a ser verificado
 * @param sufixo Sufixo esperado
 * 
 */
bool arquivo_termina_com(const char *texto, const char *sufixo);

/**
 * @brief Extrai o nome base de um arquivo removendo caminho e extensão.
 *
 * @param caminho_ou_nome Caminho completo ou nome simples do arquivo
 * @param extensao Extensão esperada no final do nome
 */
char *arquivo_extrair_nome_base(const char *caminho_ou_nome, const char *extensao);

/**
 * @brief Monta um nome composto a partir de duas bases e um separador.
 *
 *
 * @param base1 Primeira parte do nome
 * @param separador Texto usado entre `base1` e `base2`
 * @param base2 Segunda parte do nome
 */
char *arquivo_montar_nome_composto(const char *base1, const char *separador,
                                   const char *base2);

/**
 * @brief Monta o caminho completo de um arquivo de saída.
 *
 *
 * @param diretorio Diretório onde o arquivo será criado
 * @param nome_base Nome do arquivo sem extensão
 * @param sufixo Sufixo final, tipicamente a extensão do arquivo
 */
char *arquivo_montar_caminho_saida(const char *diretorio, const char *nome_base,
                                   const char *sufixo);

#endif
