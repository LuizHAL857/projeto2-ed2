#include "../unity/unity.h"
#include "../include/hash_table.h"

#include <stdlib.h>

typedef struct {
    int quantidade;
    int soma;
} AcumuladorVisita;

static HashTable tabela;
static int destruicoes;

static int *criaInteiro(int valor) {

    int *numero = malloc(sizeof(int));

    if (numero != NULL) {
        *numero = valor;
    }

    return numero;
}

static void destruirInteiro(void *valor) {
    if (valor != NULL) {
        destruicoes++;
        free(valor);
    }
}

static void acumularPares(const char *chave, void *valor, void *extra) {

    AcumuladorVisita *acumulador = extra;
    int *numero = valor;

    TEST_ASSERT_NOT_NULL(chave);
    TEST_ASSERT_NOT_NULL(valor);
    TEST_ASSERT_NOT_NULL(extra);

    acumulador->quantidade++;
    acumulador->soma += *numero;
}

void setUp(void) {

    tabela = criaHashTable(11);
    destruicoes = 0;
}

void tearDown(void) {

    liberaHashTable(tabela, destruirInteiro);
    tabela = NULL;
    destruicoes = 0;
}

void test_criaHashTable_deve_iniciar_vazia(void) {

    TEST_ASSERT_NOT_NULL(tabela);
    TEST_ASSERT_EQUAL_INT(0, getTamanhoHashTable(tabela));
    TEST_ASSERT_FALSE(chaveExisteHashTable(tabela, "cep1"));
    TEST_ASSERT_NULL(buscaHashTable(tabela, "cep1"));
}

void test_insereHashTable_deve_permitir_busca_por_chave(void) {

    int *valor = criaInteiro(42);

    TEST_ASSERT_NOT_NULL(valor);
    TEST_ASSERT_TRUE(insereHashTable(tabela, "cep1", valor));
    TEST_ASSERT_EQUAL_INT(1, getTamanhoHashTable(tabela));
    TEST_ASSERT_TRUE(chaveExisteHashTable(tabela, "cep1"));
    TEST_ASSERT_EQUAL_PTR(valor, buscaHashTable(tabela, "cep1"));
    TEST_ASSERT_EQUAL_INT(42, *(int *)buscaHashTable(tabela, "cep1"));
}

void test_insereHashTable_deve_atualizar_valor_sem_duplicar_chave(void) {

    static int valor_antigo = 10;
    static int valor_novo = 25;

    TEST_ASSERT_TRUE(insereHashTable(tabela, "cep1", &valor_antigo));
    TEST_ASSERT_TRUE(insereHashTable(tabela, "cep1", &valor_novo));

    TEST_ASSERT_EQUAL_INT(1, getTamanhoHashTable(tabela));
    TEST_ASSERT_EQUAL_PTR(&valor_novo, buscaHashTable(tabela, "cep1"));

    liberaHashTable(tabela, NULL);
    tabela = NULL;
}

void test_removeHashTable_deve_remover_e_retornar_valor_associado(void) {

    int *valor = criaInteiro(99);
    void *removido;

    TEST_ASSERT_NOT_NULL(valor);
    TEST_ASSERT_TRUE(insereHashTable(tabela, "cep1", valor));

    removido = removeHashTable(tabela, "cep1");

    TEST_ASSERT_EQUAL_PTR(valor, removido);
    TEST_ASSERT_EQUAL_INT(0, getTamanhoHashTable(tabela));
    TEST_ASSERT_FALSE(chaveExisteHashTable(tabela, "cep1"));
    TEST_ASSERT_NULL(buscaHashTable(tabela, "cep1"));

    free(removido);
}

void test_removeHashTable_deve_retornar_null_quando_chave_nao_existe(void) {

    TEST_ASSERT_NULL(removeHashTable(tabela, "inexistente"));
    TEST_ASSERT_EQUAL_INT(0, getTamanhoHashTable(tabela));
}

void test_percorreHashTable_deve_visitar_todos_os_pares_armazenados(void) {

    static int valor_a = 10;
    static int valor_b = 20;
    static int valor_c = 30;
    AcumuladorVisita acumulador = {0, 0};

    TEST_ASSERT_TRUE(insereHashTable(tabela, "cep1", &valor_a));
    TEST_ASSERT_TRUE(insereHashTable(tabela, "cep2", &valor_b));
    TEST_ASSERT_TRUE(insereHashTable(tabela, "cep3", &valor_c));

    percorreHashTable(tabela, acumularPares, &acumulador);

    TEST_ASSERT_EQUAL_INT(3, acumulador.quantidade);
    TEST_ASSERT_EQUAL_INT(60, acumulador.soma);

    liberaHashTable(tabela, NULL);
    tabela = NULL;
}

void test_liberaHashTable_deve_chamar_callback_para_cada_valor_armazenado(void) {

    int *valor_a = criaInteiro(7);
    int *valor_b = criaInteiro(14);

    TEST_ASSERT_NOT_NULL(valor_a);
    TEST_ASSERT_NOT_NULL(valor_b);
    TEST_ASSERT_TRUE(insereHashTable(tabela, "cep1", valor_a));
    TEST_ASSERT_TRUE(insereHashTable(tabela, "cep2", valor_b));

    liberaHashTable(tabela, destruirInteiro);
    tabela = NULL;

    TEST_ASSERT_EQUAL_INT(2, destruicoes);
}

int main(void) {
    
    UNITY_BEGIN();

    RUN_TEST(test_criaHashTable_deve_iniciar_vazia);
    RUN_TEST(test_insereHashTable_deve_permitir_busca_por_chave);
    RUN_TEST(test_insereHashTable_deve_atualizar_valor_sem_duplicar_chave);
    RUN_TEST(test_removeHashTable_deve_remover_e_retornar_valor_associado);
    RUN_TEST(test_removeHashTable_deve_retornar_null_quando_chave_nao_existe);
    RUN_TEST(test_percorreHashTable_deve_visitar_todos_os_pares_armazenados);
    RUN_TEST(test_liberaHashTable_deve_chamar_callback_para_cada_valor_armazenado);

    return UNITY_END();
}
