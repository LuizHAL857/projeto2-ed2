#ifndef QUADRA_H
#define QUADRA_H

#include <stdbool.h>

/*
 * Modulo de representacao das quadras da cidade.
 *
 * Cada quadra e identificada por um CEP textual e representada geometricamente
 * por um retangulo definido por sua posicao, largura e altura. O modulo tambem
 * armazena os atributos visuais definidos pelo comando `cq` do arquivo `.geo`.
 *
 * As quadras podem ser armazenadas em uma HashTable usando o CEP como chave.
 * A funcao liberaQuadra e compativel com o callback HashDestruirValor.
 */

/**
 * @brief Tipo opaco para uma quadra.
 */
typedef void* Quadra;

/**
 * @brief Cria uma quadra.
 *
 * O CEP e copiado internamente. A largura e a altura devem ser positivas.
 *
 * @param cep Identificador textual da quadra
 * @param x Coordenada x da quadra
 * @param y Coordenada y da quadra
 * @param largura Largura da quadra
 * @param altura Altura da quadra
 * @return Quadra criada ou NULL em caso de parametros invalidos ou erro
 */
Quadra criaQuadra(const char *cep, float x, float y, float largura, float altura);

/**
 * @brief Libera toda a memoria associada a uma quadra.
 *
 * @param quadra Quadra a ser liberada
 */
void liberaQuadra(void *quadra);

/**
 * @brief Define os atributos visuais da quadra.
 *
 * As cores sao copiadas internamente.
 *
 * @param quadra Quadra que recebera o estilo
 * @param espessuraBorda Espessura da borda
 * @param corPreenchimento Cor de preenchimento
 * @param corBorda Cor da borda
 * @return true em caso de sucesso, false para parametros invalidos
 */
bool setEstiloQuadra(Quadra quadra, float espessuraBorda,
                     const char *corPreenchimento, const char *corBorda);

/**
 * @brief Retorna o CEP da quadra.
 */
const char *getCepQuadra(Quadra quadra);

/**
 * @brief Retorna a coordenada x da quadra.
 */
float getXQuadra(Quadra quadra);

/**
 * @brief Retorna a coordenada y da quadra.
 */
float getYQuadra(Quadra quadra);

/**
 * @brief Retorna a largura da quadra.
 */
float getLarguraQuadra(Quadra quadra);

/**
 * @brief Retorna a altura da quadra.
 */
float getAlturaQuadra(Quadra quadra);

/**
 * @brief Retorna a espessura da borda da quadra.
 */
float getEspessuraBordaQuadra(Quadra quadra);

/**
 * @brief Retorna a cor de preenchimento da quadra.
 */
const char *getCorPreenchimentoQuadra(Quadra quadra);

/**
 * @brief Retorna a cor da borda da quadra.
 */
const char *getCorBordaQuadra(Quadra quadra);

#endif
