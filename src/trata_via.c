#include "../include/trata_via.h"

#include "../include/hash_table.h"

#include <stdlib.h>
#include <string.h>

#define TAMANHO_ID_VERTICE 256
#define TAMANHO_TEXTO_ARESTA 256

typedef struct stDadosVia {
    Grafo grafo;
    HashTable verticesPorId;
} stDadosVia;

static bool processaComandoVertice(const char *linha, DadosVia dadosVia) {
    stDadosVia *dados = dadosVia;
    char id[TAMANHO_ID_VERTICE];
    float x;
    float y;
    Vertice vertice;

    if (sscanf(linha, " v %255s %f %f", id, &x, &y) != 3 ||
        chaveExisteHashTable(dados->verticesPorId, id)) {
        return false;
    }

    vertice = infoVertice(dados->grafo, id, x, y);
    if (vertice == NULL) {
        return false;
    }

    if (!insereHashTable(dados->verticesPorId, id, vertice)) {
        return false;
    }

    return true;
}

static bool processaComandoAresta(const char *linha, DadosVia dadosVia) {
    stDadosVia *dados = dadosVia;
    char origemId[TAMANHO_ID_VERTICE];
    char destinoId[TAMANHO_ID_VERTICE];
    char ldir[TAMANHO_TEXTO_ARESTA];
    char lesq[TAMANHO_TEXTO_ARESTA];
    char nome[TAMANHO_TEXTO_ARESTA];
    float comprimento;
    float velocidadeMedia;
    Vertice origem;
    Vertice destino;
    Aresta aresta;

    if (sscanf(linha, " e %255s %255s %255s %255s %f %f %255s",
               origemId, destinoId, ldir, lesq,
               &comprimento, &velocidadeMedia, nome) != 7) {
        return false;
    }

    origem = buscaHashTable(dados->verticesPorId, origemId);
    destino = buscaHashTable(dados->verticesPorId, destinoId);

    if (origem == NULL || destino == NULL ||
        arestaExiste(dados->grafo, origem, destino)) {
        return false;
    }

    aresta = insereAresta(dados->grafo, origem, destino);
    if (aresta == NULL) {
        return false;
    }

    setInfoAresta(dados->grafo, aresta, nome, ldir, lesq,
                  comprimento, velocidadeMedia);
    return true;
}

static bool processaLinhaVia(const char *linha, DadosVia dadosVia) {
    char comando[2];

    if (sscanf(linha, " %1s", comando) != 1) {
        return false;
    }

    if (strcmp(comando, "v") == 0) {
        return processaComandoVertice(linha, dadosVia);
    }

    if (strcmp(comando, "e") == 0) {
        return processaComandoAresta(linha, dadosVia);
    }

    return false;
}

DadosVia tratar_arquivo_via(DadosDoArquivo dadosViaArquivo) {
    Lista linhas;
    Celula atual;
    const char *primeiraLinha;
    int quantidadeVertices;
    stDadosVia *dados;

    if (dadosViaArquivo == NULL ||
        !arquivo_termina_com(obter_nome_arquivo(dadosViaArquivo), ".via")) {
        return NULL;
    }

    linhas = obter_lista_linhas(dadosViaArquivo);
    if (linhas == NULL || getInicioLista(linhas) == NULL) {
        return NULL;
    }

    primeiraLinha = getConteudoCelula(getInicioLista(linhas));
    if (sscanf(primeiraLinha, " %d", &quantidadeVertices) != 1 ||
        quantidadeVertices <= 0) {
        return NULL;
    }

    dados = malloc(sizeof(stDadosVia));
    if (dados == NULL) {
        return NULL;
    }

    dados->grafo = criaGrafo(quantidadeVertices);
    dados->verticesPorId = criaHashTable(quantidadeVertices * 2 + 1);

    if (dados->grafo == NULL || dados->verticesPorId == NULL) {
        desaloca_via(dados);
        return NULL;
    }

    atual = getProxCelula(getInicioLista(linhas));
    while (atual != NULL) {
        const char *linha = getConteudoCelula(atual);

        if (!processaLinhaVia(linha, dados)) {
            desaloca_via(dados);
            return NULL;
        }

        atual = getProxCelula(atual);
    }

    return dados;
}

void desaloca_via(DadosVia dadosVia) {
    stDadosVia *dados = dadosVia;

    if (dados == NULL) {
        return;
    }

    liberaHashTable(dados->verticesPorId, NULL);
    liberaGrafo(dados->grafo);
    free(dados);
}

Grafo obter_grafo_via(DadosVia dadosVia) {
    stDadosVia *dados = dadosVia;

    if (dados == NULL) {
        return NULL;
    }

    return dados->grafo;
}

Vertice obter_vertice_via(DadosVia dadosVia, const char *idVertice) {
    stDadosVia *dados = dadosVia;

    if (dados == NULL || idVertice == NULL) {
        return NULL;
    }

    return buscaHashTable(dados->verticesPorId, idVertice);
}
