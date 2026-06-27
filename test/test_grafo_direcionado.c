#include "../unity/unity.h"
#include "../include/grafo_direcionado.h"
#include "../include/lista.h"

static Grafo grafo;
static Vertice v1;
static Vertice v2;
static Vertice v3;
static Vertice v4;

void setUp(void) {
    grafo = criaGrafo(4);
    v1 = infoVertice(grafo, "v1", 10.0f, 20.0f);
    v2 = infoVertice(grafo, "v2", 30.0f, 40.0f);
    v3 = infoVertice(grafo, "v3", 50.0f, 60.0f);
    v4 = infoVertice(grafo, "v4", 70.0f, 80.0f);
}

void tearDown(void) {
    liberaGrafo(grafo);
    grafo = NULL;
    v1 = NULL;
    v2 = NULL;
    v3 = NULL;
    v4 = NULL;
}

void test_criaGrafo_e_infoVertice_devem_retornar_ponteiros_validos(void) {
    TEST_ASSERT_NOT_NULL(grafo);
    TEST_ASSERT_NOT_NULL(v1);
    TEST_ASSERT_NOT_NULL(v2);
    TEST_ASSERT_NOT_NULL(v3);
    TEST_ASSERT_NOT_NULL(v4);
    TEST_ASSERT_EQUAL_STRING("v1", getIdVertice(v1));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 10.0f, getXVertice(v1));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 20.0f, getYVertice(v1));
}

void test_arestaExiste_deve_retornar_false_quando_aresta_ainda_nao_foi_inserida(void) {
    TEST_ASSERT_FALSE(arestaExiste(grafo, v1, v2));
    TEST_ASSERT_FALSE(adjacentes(grafo, v1, v2));
}

void test_insereAresta_deve_registrar_origem_destino_e_adjacencia_direcionada(void) {
    Aresta aresta = insereAresta(grafo, v1, v2);

    TEST_ASSERT_NOT_NULL(aresta);
    TEST_ASSERT_TRUE(arestaExiste(grafo, v1, v2));
    TEST_ASSERT_FALSE(arestaExiste(grafo, v2, v1));
    TEST_ASSERT_TRUE(adjacentes(grafo, v1, v2));
    TEST_ASSERT_FALSE(adjacentes(grafo, v2, v1));
    TEST_ASSERT_EQUAL_PTR(v1, getVerticeOrigemAresta(aresta));
    TEST_ASSERT_EQUAL_PTR(v2, getVerticeDestinoAresta(aresta));
}

void test_setInfoAresta_e_getters_devem_preservar_os_dados_da_aresta(void) {
    Aresta aresta = insereAresta(grafo, v2, v3);

    TEST_ASSERT_NOT_NULL(aresta);

    setInfoAresta(
        grafo,
        aresta,
        "Rua dos lírios",
        "cep-dir",
        "cep-esq",
        125.5f,
        13.75f
    );

    TEST_ASSERT_EQUAL_STRING("Rua dos lírios", getNomeAresta(aresta));
    TEST_ASSERT_EQUAL_STRING("cep-dir", getLdirAresta(aresta));
    TEST_ASSERT_EQUAL_STRING("cep-esq", getLesqAresta(aresta));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 125.5f, getComprimentoAresta(aresta));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 13.75f, getVelocidadeMediaAresta(aresta));
}

void test_removeAresta_deve_remover_apenas_o_sentido_informado(void) {
    TEST_ASSERT_NOT_NULL(insereAresta(grafo, v1, v2));
    TEST_ASSERT_NOT_NULL(insereAresta(grafo, v2, v1));

    TEST_ASSERT_TRUE(arestaExiste(grafo, v1, v2));
    TEST_ASSERT_TRUE(arestaExiste(grafo, v2, v1));

    TEST_ASSERT_TRUE(removeAresta(grafo, v1, v2));
    TEST_ASSERT_FALSE(arestaExiste(grafo, v1, v2));
    TEST_ASSERT_TRUE(arestaExiste(grafo, v2, v1));
    TEST_ASSERT_FALSE(adjacentes(grafo, v1, v2));
    TEST_ASSERT_TRUE(adjacentes(grafo, v2, v1));
}

void test_removeAresta_deve_retornar_false_quando_aresta_nao_existe(void) {
    TEST_ASSERT_FALSE(removeAresta(grafo, v3, v4));
}

void test_removeAresta_deve_impedir_nova_consulta_de_existencia_apos_remocao(void) {
    TEST_ASSERT_NOT_NULL(insereAresta(grafo, v3, v4));
    TEST_ASSERT_TRUE(arestaExiste(grafo, v3, v4));

    TEST_ASSERT_TRUE(removeAresta(grafo, v3, v4));
    TEST_ASSERT_FALSE(arestaExiste(grafo, v3, v4));
}

void test_liberaGrafo_deve_aceitar_grafo_com_vertices_e_arestas(void) {
    Aresta aresta1 = insereAresta(grafo, v1, v2);
    Aresta aresta2 = insereAresta(grafo, v2, v3);

    TEST_ASSERT_NOT_NULL(aresta1);
    TEST_ASSERT_NOT_NULL(aresta2);

    setInfoAresta(grafo, aresta1, "Rua A", "qd1", "qd2", 10.0f, 5.0f);
    setInfoAresta(grafo, aresta2, "Rua B", "qd3", "qd4", 20.0f, 6.0f);

    liberaGrafo(grafo);
    grafo = NULL;
    v1 = NULL;
    v2 = NULL;
    v3 = NULL;
    v4 = NULL;

    TEST_ASSERT_TRUE(true);
}


int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_criaGrafo_e_infoVertice_devem_retornar_ponteiros_validos);
    RUN_TEST(test_arestaExiste_deve_retornar_false_quando_aresta_ainda_nao_foi_inserida);
    RUN_TEST(test_insereAresta_deve_registrar_origem_destino_e_adjacencia_direcionada);
    RUN_TEST(test_setInfoAresta_e_getters_devem_preservar_os_dados_da_aresta);
    RUN_TEST(test_removeAresta_deve_remover_apenas_o_sentido_informado);
    RUN_TEST(test_removeAresta_deve_retornar_false_quando_aresta_nao_existe);
    RUN_TEST(test_removeAresta_deve_impedir_nova_consulta_de_existencia_apos_remocao);
    RUN_TEST(test_liberaGrafo_deve_aceitar_grafo_com_vertices_e_arestas);

    return UNITY_END();
}
