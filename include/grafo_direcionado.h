#ifndef GRAFO_DIRECIONADO_H
#define GRAFO_DIRECIONADO_H

#include <stdbool.h>
#include <lista.h>

/*
 * Módulo de grafo direcionado para representar o sistema viário da cidade.
 *
 * O grafo modela o mapa viário descrito no projeto: vértices representam
 * extremos de segmentos de rua e arestas representam trechos orientados de
 * circulação entre dois vértices. Cada aresta pode armazenar atributos
 * associados ao trecho, como nome da rua, quadras laterais, comprimento e
 * velocidade média.
 *
 * A implementação do módulo usa listas de adjacência, o que
 * permite representar o grafo de forma dinâmica e eficiente para consultas 
 *  inserção de trechos, buscas e algoritmos de caminhos mínimos.
 */


/** 
* @brief Tipo opaco para representação de um grafo direcionado
*/
typedef void* Grafo;

/**
 *  @brief Tipo opaco para representação de uma aresta de um grafo
 */
typedef void* Aresta;

/**
 * @brief Tipo opaco para representação de um vértice de um grafo
 */
typedef void* Vertice;


/**
 * @brief cria um grafo vazio com n vértices e arestas iniciais
* @param n quantidade de vértices
 */
Grafo criaGrafo(int n);

/**
 * @brief Libera toda a memória associada ao grafo
 *
 * @param grafo Grafo a ser liberado
 */
void liberaGrafo(Grafo grafo);

/**
 * @brief Insere informações em um vértice
 * @param grafo Grafo em que vértice terá sua informação colocada
 * @param id Identificador do vértice 
 * @param x Coordenada x do vértice
 * @param y Coordenada y do vértice
 */
Vertice infoVertice(Grafo grafo,int id, float x, float y);

/**
 * @brief Insere uma aresta em um grafo tendo como referência dois vértices
 * 
 * @param grafo Grafo em que a aresta será adicionada
 * @param v1 Vértice de "origem" da aresta
 * @param v2 Vértice de "destino" da aresta
 */
Aresta insereAresta(Grafo grafo, Vertice v1, Vertice v2);

/**
 * @brief Verifica se há uma aresta com os respectivos vértices no grafo desejado
 * 
 * @param grafo Grafo em que os vértices serão analisados
 * @param v1 Vértice de origem da "aresta" a ser analisada
 * @param v2 Vértice de destino da "aresta" a ser analisada
 * 
 * 
 */
bool arestaExiste(Grafo grafo, Vertice v1, Vertice v2);

/**
 * @brief Define as informações de uma aresta
 * 
 * @param grafo Grafo em que a aresta está
 * @param aresta Aresta que terá suas informações definidas 
 * @param nome nome da rua a qual pertence o segmento
 * @param ldir Cep da quadra que está a direita do segmento
 * @param lesq Cep da quadra que está a esquerda do segmento
 * @param comprimento comprimento (em mentros) do segmento
 * @param velocidadeMedia Velocidade média (m/s) que os carros trafegam no segmento
 */
void setInfoAresta(Grafo grafo, Aresta aresta, char* nome, char* ldir, char* lesq, float comprimento, float velocidadeMedia);

/**
 * @brief Retorna o vertice de origem de uma aresta
 */
Vertice getVerticeOrigemAresta(Aresta aresta);

/**
 * @brief Retorna o vertice de destino de uma aresta
 */
Vertice getVerticeDestinoAresta(Aresta aresta);

/**
 * @brief Retorna o nome da rua associado a uma aresta
 *
 * @param aresta Aresta desejada
 * @return Nome da rua da aresta
 */
char* getNomeAresta(Aresta aresta);

/**
 * @brief Retorna o CEP da quadra à direita de uma aresta
 *
 * @param aresta Aresta desejada
 * @return CEP da quadra à direita
 */
char* getLdirAresta(Aresta aresta);

/**
 * @brief Retorna o CEP da quadra à esquerda de uma aresta
 *
 * @param aresta Aresta desejada
 * @return CEP da quadra à esquerda
 */
char* getLesqAresta(Aresta aresta);

/**
 * @brief Retorna o comprimento de uma aresta em metros
 *
 * @param aresta Aresta desejada
 * @return Comprimento da aresta
 */
float getComprimentoAresta(Aresta aresta);

/**
 * @brief Retorna a velocidade média de uma aresta em m/s
 *
 * @param aresta Aresta desejada
 * @return Velocidade média da aresta
 */
float getVelocidadeMediaAresta(Aresta aresta);

/**
 * @brief Remove uma aresta do grafo a partir de seus vértices
 *
 * @param grafo Grafo em que a aresta está
 * @param v1 Vértice de origem da aresta
 * @param v2 Vértice de destino da aresta
 * @return true se a aresta foi removida, false caso contrário
 */
bool removeAresta(Grafo grafo, Vertice v1, Vertice v2);


/**
 * @brief Analisa se dois vertices são adjacentes em um grafo
 * 
 * @param grafo Grafo em que os vertices serão analisados
 * @param v1 vértice 1 a ser analisado
 * @param v2 vértice 2 a ser analisado
 */
bool adjacentes(Grafo grafo, Vertice v1, Vertice v2);

/**
 * @brief Retorna uma lista com vértices adjacentes de um grafos
 * 
 * @param grafo a ser analisado
 */
Lista verticesAdjacentesLista(Grafo grafo);


#endif
