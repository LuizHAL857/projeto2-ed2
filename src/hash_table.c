#include "../include/hash_table.h"

#include <stdlib.h>
#include <string.h>

typedef struct stHashNo {

    char *chave;
    void *valor;
    struct stHashNo *prox;

} stHashNo;

typedef struct stHashTableImpl {

    int capacidade;
    int tamanho;
    stHashNo **buckets;

} stHashTableImpl;

static unsigned long calculaHash(const char *texto) {

    unsigned long hash = 5381;
    unsigned char c;

    if (texto == NULL) {
        return 0;
    }

    while ((c = (unsigned char)*texto++) != 0) {

        hash = ((hash << 5) + hash) + c;

    }

    return hash;
}

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

static stHashNo *buscaNo(stHashTableImpl *tabela, const char *chave, int *indiceSaida) {

    unsigned long hash;
    int indice;
    stHashNo *atual;

    if (tabela == NULL || chave == NULL) {

        return NULL;
    }

    hash = calculaHash(chave);
    indice = (int)(hash % (unsigned long)tabela->capacidade);

    if (indiceSaida != NULL) {

        *indiceSaida = indice;
    }

    atual = tabela->buckets[indice];

    while (atual != NULL) {
        if (strcmp(atual->chave, chave) == 0) {
            return atual;
        }
        atual = atual->prox;
    }

    return NULL;

}

static bool redimensionaHashTable(stHashTableImpl *tabela, int novaCapacidade) {
    
    stHashNo **novosBuckets;
    int i;

    if (tabela == NULL || novaCapacidade <= tabela->capacidade) {

        return false;
    }

    novosBuckets = calloc((size_t)novaCapacidade, sizeof(stHashNo *));
    if (novosBuckets == NULL) {

        return false;
    }

    for (i = 0; i < tabela->capacidade; i++) {
        stHashNo *atual = tabela->buckets[i];

        while (atual != NULL) {

            stHashNo *proximo = atual->prox;
            unsigned long hash = calculaHash(atual->chave);
            int novoIndice = (int)(hash % (unsigned long)novaCapacidade);

            atual->prox = novosBuckets[novoIndice];
            novosBuckets[novoIndice] = atual;
            atual = proximo;
        }
    }

    free(tabela->buckets);
    tabela->buckets = novosBuckets;
    tabela->capacidade = novaCapacidade;

    return true;
}

HashTable criaHashTable(int capacidadeInicial) {

    stHashTableImpl *tabela;

    if (capacidadeInicial <= 0) {
        return NULL;
    }

    tabela = malloc(sizeof(stHashTableImpl));
    if (tabela == NULL) {
        return NULL;
    }

    tabela->buckets = calloc((size_t)capacidadeInicial, sizeof(stHashNo *));
    if (tabela->buckets == NULL) {
        free(tabela);
        return NULL;
    }

    tabela->capacidade = capacidadeInicial;
    tabela->tamanho = 0;

    return tabela;
}

void liberaHashTable(HashTable tabela, HashDestruirValor destruirValor) {

    stHashTableImpl *hash = tabela;
    int i;

    if (hash == NULL) {
        return;
    }

    for (i = 0; i < hash->capacidade; i++) {
        stHashNo *atual = hash->buckets[i];

        while (atual != NULL) {
            stHashNo *proximo = atual->prox;

            if (destruirValor != NULL) {
                destruirValor(atual->valor);
            }

            free(atual->chave);
            free(atual);
            atual = proximo;
        }
    }

    free(hash->buckets);
    free(hash);
}

bool insereHashTable(HashTable tabela, const char *chave, void *valor) {

    stHashTableImpl *hash = tabela;
    stHashNo *existente;
    stHashNo *novoNo;
    char *chaveCopia;
    int indice;

    if (hash == NULL || chave == NULL) {
        return false;
    }

    existente = buscaNo(hash, chave, &indice);
    if (existente != NULL) {
        existente->valor = valor;
        return true;
    }

    if ((hash->tamanho + 1) * 4 > hash->capacidade * 3) {
        redimensionaHashTable(hash, hash->capacidade * 2 + 1);
        buscaNo(hash, chave, &indice);
    }

    chaveCopia = copiaString(chave);
    if (chaveCopia == NULL) {
        return false;
    }

    novoNo = malloc(sizeof(stHashNo));
    if (novoNo == NULL) {
        free(chaveCopia);
        return false;
    }

    novoNo->chave = chaveCopia;
    novoNo->valor = valor;
    novoNo->prox = hash->buckets[indice];
    hash->buckets[indice] = novoNo;
    hash->tamanho++;

    return true;
}

void *buscaHashTable(HashTable tabela, const char *chave) {

    stHashTableImpl *hash = tabela;
    stHashNo *no;

    no = buscaNo(hash, chave, NULL);
    if (no == NULL) {

        return NULL;
    }

    return no->valor;
}

bool chaveExisteHashTable(HashTable tabela, const char *chave) {

    stHashTableImpl *hash = tabela;
    return buscaNo(hash, chave, NULL) != NULL;
}

void *removeHashTable(HashTable tabela, const char *chave) {

    stHashTableImpl *hash = tabela;
    unsigned long hashChave;
    int indice;
    stHashNo *atual;
    stHashNo *anterior;
    void *valor;

    if (hash == NULL || chave == NULL) {
        return NULL;
    }

    hashChave = calculaHash(chave);
    indice = (int)(hashChave % (unsigned long)hash->capacidade);

    anterior = NULL;
    atual = hash->buckets[indice];

    while (atual != NULL) {

        if (strcmp(atual->chave, chave) == 0) {
            if (anterior == NULL) {
                hash->buckets[indice] = atual->prox;
            } else {
                anterior->prox = atual->prox;
            }

            valor = atual->valor;
            free(atual->chave);
            free(atual);
            hash->tamanho--;
            return valor;
        }

        anterior = atual;
        atual = atual->prox;
    }

    return NULL;
}

int getTamanhoHashTable(HashTable tabela) {

    stHashTableImpl *hash = tabela;

    if (hash == NULL) {
        return 0;
    }

    return hash->tamanho;
}

void percorreHashTable(HashTable tabela, HashVisitaPar visita, void *extra) {

    stHashTableImpl *hash = tabela;
    int i;

    if (hash == NULL || visita == NULL) {
        return;
    }

    for (i = 0; i < hash->capacidade; i++) {
        stHashNo *atual = hash->buckets[i];

        while (atual != NULL) {
            visita(atual->chave, atual->valor, extra);
            atual = atual->prox;
        }
    }
}
