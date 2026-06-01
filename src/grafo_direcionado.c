#include "../include/grafo_direcionado.h"

#include <stdlib.h>
#include <string.h>

typedef struct stVerticeImpl stVerticeImpl;
typedef struct stArestaImpl stArestaImpl;

struct stArestaImpl {
    stVerticeImpl *origem;
    stVerticeImpl *destino;
    char *nome;
    char *ldir;
    char *lesq;
    float comprimento;
    float velocidadeMedia;
};

struct stVerticeImpl {
    int id;
    float x;
    float y;
    Lista arestasSaida;
};

typedef struct stGrafoImpl {
    int capacidadeVertices;
    int quantidadeVertices;
    Lista vertices;
} stGrafoImpl;

static char *copiaString(const char *texto) {
    char *copia;
    size_t tamanho;

    if (texto == NULL) {
        return NULL;
    }

    tamanho = strlen(texto) + 1;
    copia = malloc(tamanho);
    if (copia == NULL) {
        return NULL;
    }

    memcpy(copia, texto, tamanho);
    return copia;
}

static void liberaCampoString(char **campo) {
    if (campo == NULL || *campo == NULL) {
        return;
    }

    free(*campo);
    *campo = NULL;
}

static stArestaImpl *buscaAresta(stVerticeImpl *origem, stVerticeImpl *destino) {
    Celula atual;

    if (origem == NULL || destino == NULL) {
        return NULL;
    }

    atual = getInicioLista(origem->arestasSaida);
    while (atual != NULL) {
        stArestaImpl *aresta = getConteudoCelula(atual);
        if (aresta != NULL && aresta->destino == destino) {
            return aresta;
        }
        atual = getProxCelula(atual);
    }

    return NULL;
}

static void liberaAresta(stArestaImpl *aresta) {
    if (aresta == NULL) {
        return;
    }

    liberaCampoString(&aresta->nome);
    liberaCampoString(&aresta->ldir);
    liberaCampoString(&aresta->lesq);
    free(aresta);
}

static bool contemVerticeNaLista(Lista lista, Vertice vertice) {
    Celula atual;

    if (lista == NULL || vertice == NULL) {
        return false;
    }

    atual = getInicioLista(lista);
    while (atual != NULL) {
        if (getConteudoCelula(atual) == vertice) {
            return true;
        }
        atual = getProxCelula(atual);
    }

    return false;
}

Grafo criaGrafo(int n) {
    stGrafoImpl *grafo;

    if (n <= 0) {
        return NULL;
    }

    grafo = malloc(sizeof(stGrafoImpl));
    if (grafo == NULL) {
        return NULL;
    }

    grafo->vertices = criaLista();
    if (grafo->vertices == NULL) {
        free(grafo);
        return NULL;
    }

    grafo->capacidadeVertices = n;
    grafo->quantidadeVertices = 0;

    return grafo;
}

Vertice infoVertice(Grafo grafo, int id, float x, float y) {
    stGrafoImpl *grafoImpl = grafo;
    stVerticeImpl *vertice;

    if (grafoImpl == NULL || grafoImpl->quantidadeVertices >= grafoImpl->capacidadeVertices) {
        return NULL;
    }

    vertice = malloc(sizeof(stVerticeImpl));
    if (vertice == NULL) {
        return NULL;
    }

    vertice->id = id;
    vertice->x = x;
    vertice->y = y;
    vertice->arestasSaida = criaLista();

    if (vertice->arestasSaida == NULL) {
        free(vertice);
        return NULL;
    }

    insereFinalLista(grafoImpl->vertices, vertice);
    grafoImpl->quantidadeVertices++;

    return vertice;
}

Aresta insereAresta(Grafo grafo, Vertice v1, Vertice v2) {
    stGrafoImpl *grafoImpl = grafo;
    stVerticeImpl *origem = v1;
    stVerticeImpl *destino = v2;
    stArestaImpl *existente;
    stArestaImpl *novaAresta;

    if (grafoImpl == NULL || origem == NULL || destino == NULL) {
        return NULL;
    }

    existente = buscaAresta(origem, destino);
    if (existente != NULL) {
        return existente;
    }

    novaAresta = malloc(sizeof(stArestaImpl));
    if (novaAresta == NULL) {
        return NULL;
    }

    novaAresta->origem = origem;
    novaAresta->destino = destino;
    novaAresta->nome = NULL;
    novaAresta->ldir = NULL;
    novaAresta->lesq = NULL;
    novaAresta->comprimento = 0.0f;
    novaAresta->velocidadeMedia = 0.0f;

    insereFinalLista(origem->arestasSaida, novaAresta);

    return novaAresta;
}

bool arestaExiste(Grafo grafo, Vertice v1, Vertice v2) {
    (void)grafo;
    return buscaAresta(v1, v2) != NULL;
}

void setInfoAresta(Grafo grafo, Aresta aresta, char *nome, char *ldir, char *lesq, float comprimento, float velocidadeMedia) {
    stArestaImpl *arestaImpl = aresta;
    char *novoNome;
    char *novoLdir;
    char *novoLesq;

    (void)grafo;

    if (arestaImpl == NULL) {
        return;
    }

    novoNome = copiaString(nome);
    novoLdir = copiaString(ldir);
    novoLesq = copiaString(lesq);

    liberaCampoString(&arestaImpl->nome);
    liberaCampoString(&arestaImpl->ldir);
    liberaCampoString(&arestaImpl->lesq);

    arestaImpl->nome = novoNome;
    arestaImpl->ldir = novoLdir;
    arestaImpl->lesq = novoLesq;
    arestaImpl->comprimento = comprimento;
    arestaImpl->velocidadeMedia = velocidadeMedia;
}

Vertice getVerticeOrigemAresta(Aresta aresta) {
    stArestaImpl *arestaImpl = aresta;

    if (arestaImpl == NULL) {
        return NULL;
    }

    return arestaImpl->origem;
}

Vertice getVerticeDestinoAresta(Aresta aresta) {
    stArestaImpl *arestaImpl = aresta;

    if (arestaImpl == NULL) {
        return NULL;
    }

    return arestaImpl->destino;
}

char *getNomeAresta(Aresta aresta) {
    stArestaImpl *arestaImpl = aresta;

    if (arestaImpl == NULL) {
        return NULL;
    }

    return arestaImpl->nome;
}

char *getLdirAresta(Aresta aresta) {
    stArestaImpl *arestaImpl = aresta;

    if (arestaImpl == NULL) {
        return NULL;
    }

    return arestaImpl->ldir;
}

char *getLesqAresta(Aresta aresta) {
    stArestaImpl *arestaImpl = aresta;

    if (arestaImpl == NULL) {
        return NULL;
    }

    return arestaImpl->lesq;
}

float getComprimentoAresta(Aresta aresta) {
    stArestaImpl *arestaImpl = aresta;

    if (arestaImpl == NULL) {
        return 0.0f;
    }

    return arestaImpl->comprimento;
}

float getVelocidadeMediaAresta(Aresta aresta) {
    stArestaImpl *arestaImpl = aresta;

    if (arestaImpl == NULL) {
        return 0.0f;
    }

    return arestaImpl->velocidadeMedia;
}

bool removeAresta(Grafo grafo, Vertice v1, Vertice v2) {
    stGrafoImpl *grafoImpl = grafo;
    stVerticeImpl *origem = v1;
    stVerticeImpl *destino = v2;
    stArestaImpl *aresta;

    if (grafoImpl == NULL || origem == NULL || destino == NULL) {
        return false;
    }

    aresta = buscaAresta(origem, destino);
    if (aresta == NULL) {
        return false;
    }

    if (!removeElementoLista(origem->arestasSaida, aresta)) {
        return false;
    }

    liberaAresta(aresta);
    return true;
}

bool adjacentes(Grafo grafo, Vertice v1, Vertice v2) {
    return arestaExiste(grafo, v1, v2);
}

Lista verticesAdjacentesLista(Grafo grafo) {
    stGrafoImpl *grafoImpl = grafo;
    Lista adjacentes;
    Celula atualVertice;

    if (grafoImpl == NULL) {
        return NULL;
    }

    adjacentes = criaLista();
    if (adjacentes == NULL) {
        return NULL;
    }

    atualVertice = getInicioLista(grafoImpl->vertices);
    while (atualVertice != NULL) {
        stVerticeImpl *vertice = getConteudoCelula(atualVertice);
        Celula atualAresta = getInicioLista(vertice->arestasSaida);

        while (atualAresta != NULL) {
            stArestaImpl *aresta = getConteudoCelula(atualAresta);
            if (aresta != NULL && !contemVerticeNaLista(adjacentes, aresta->destino)) {
                insereFinalLista(adjacentes, aresta->destino);
            }
            atualAresta = getProxCelula(atualAresta);
        }

        atualVertice = getProxCelula(atualVertice);
    }

    return adjacentes;
}

