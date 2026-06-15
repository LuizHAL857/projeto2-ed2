#include "../include/quadra.h"

#include <stdlib.h>
#include <string.h>

typedef struct stQuadra {
    char *cep;
    float x;
    float y;
    float largura;
    float altura;
    float espessuraBorda;
    char *corPreenchimento;
    char *corBorda;
} stQuadra;

static char *copiaString(const char *texto) {
    char *copia;
    size_t tamanho;

    if (texto == NULL) {
        return NULL;
    }

    tamanho = strlen(texto) + 1u;
    copia = malloc(tamanho);
    if (copia == NULL) {
        return NULL;
    }

    memcpy(copia, texto, tamanho);
    return copia;
}

Quadra criaQuadra(const char *cep, float x, float y, float largura, float altura) {
    stQuadra *quadra;

    if (cep == NULL || cep[0] == '\0' || largura <= 0.0f || altura <= 0.0f) {
        return NULL;
    }

    quadra = malloc(sizeof(stQuadra));
    if (quadra == NULL) {
        return NULL;
    }

    quadra->cep = copiaString(cep);
    if (quadra->cep == NULL) {
        free(quadra);
        return NULL;
    }

    quadra->x = x;
    quadra->y = y;
    quadra->largura = largura;
    quadra->altura = altura;
    quadra->espessuraBorda = 0.0f;
    quadra->corPreenchimento = NULL;
    quadra->corBorda = NULL;

    return quadra;
}

void liberaQuadra(void *quadra) {
    stQuadra *quadraImpl = quadra;

    if (quadraImpl == NULL) {
        return;
    }

    free(quadraImpl->cep);
    free(quadraImpl->corPreenchimento);
    free(quadraImpl->corBorda);
    free(quadraImpl);
}

bool setEstiloQuadra(Quadra quadra, float espessuraBorda,
                     const char *corPreenchimento, const char *corBorda) {
    stQuadra *quadraImpl = quadra;
    char *novoPreenchimento;
    char *novaBorda;

    if (quadraImpl == NULL || espessuraBorda < 0.0f ||
        corPreenchimento == NULL || corBorda == NULL) {
        return false;
    }

    novoPreenchimento = copiaString(corPreenchimento);
    if (novoPreenchimento == NULL) {
        return false;
    }

    novaBorda = copiaString(corBorda);
    if (novaBorda == NULL) {
        free(novoPreenchimento);
        return false;
    }

    free(quadraImpl->corPreenchimento);
    free(quadraImpl->corBorda);

    quadraImpl->espessuraBorda = espessuraBorda;
    quadraImpl->corPreenchimento = novoPreenchimento;
    quadraImpl->corBorda = novaBorda;

    return true;
}

const char *getCepQuadra(Quadra quadra) {
    stQuadra *quadraImpl = quadra;
    return quadraImpl == NULL ? NULL : quadraImpl->cep;
}

float getXQuadra(Quadra quadra) {
    stQuadra *quadraImpl = quadra;
    return quadraImpl == NULL ? 0.0f : quadraImpl->x;
}

float getYQuadra(Quadra quadra) {
    stQuadra *quadraImpl = quadra;
    return quadraImpl == NULL ? 0.0f : quadraImpl->y;
}

float getLarguraQuadra(Quadra quadra) {
    stQuadra *quadraImpl = quadra;
    return quadraImpl == NULL ? 0.0f : quadraImpl->largura;
}

float getAlturaQuadra(Quadra quadra) {
    stQuadra *quadraImpl = quadra;
    return quadraImpl == NULL ? 0.0f : quadraImpl->altura;
}

float getEspessuraBordaQuadra(Quadra quadra) {
    stQuadra *quadraImpl = quadra;
    return quadraImpl == NULL ? 0.0f : quadraImpl->espessuraBorda;
}

const char *getCorPreenchimentoQuadra(Quadra quadra) {
    stQuadra *quadraImpl = quadra;
    return quadraImpl == NULL ? NULL : quadraImpl->corPreenchimento;
}

const char *getCorBordaQuadra(Quadra quadra) {
    stQuadra *quadraImpl = quadra;
    return quadraImpl == NULL ? NULL : quadraImpl->corBorda;
}
