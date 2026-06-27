#include "../unity/unity.h"
#include "../include/grafo_direcionado.h"
#include "../include/leitor_arquivos.h"
#include "../include/trata_via.h"

#include <stdio.h>

static const char *ARQUIVO_VIA = "test_trata_via.via";

static DadosDoArquivo dadosArquivoVia;
static DadosVia dadosVia;

static void escreveViaValido(void) {
    FILE *arquivo = fopen(ARQUIVO_VIA, "w");

    TEST_ASSERT_NOT_NULL(arquivo);
    fputs("3\n", arquivo);
    fputs("v (b0|0,0) 10 10\n", arquivo);
    fputs("v (b0|0,0)(b0|0,1).0 77.5 10\n", arquivo);
    fputs("v (b0|0,1) 145 10\n", arquivo);
    fputs("e (b0|0,0) (b0|0,0)(b0|0,1).0 b01.1 # 67.5 3.841354 Al_Scoma_Way\n", arquivo);
    fputs("e (b0|0,0)(b0|0,1).0 (b0|0,1) b01.1 # 67.5 3.841354 Al_Scoma_Way\n", arquivo);
    fclose(arquivo);
}

void setUp(void) {
    escreveViaValido();
    dadosArquivoVia = criar_dados_arquivo((char *)ARQUIVO_VIA);
    dadosVia = NULL;
}

void tearDown(void) {
    desaloca_via(dadosVia);
    destruir_dados_arquivo(dadosArquivoVia);
    dadosVia = NULL;
    dadosArquivoVia = NULL;
    remove(ARQUIVO_VIA);
}

void test_tratar_arquivo_via_deve_criar_grafo_e_vertices_indexados(void) {
    Grafo grafo;
    Vertice v1;
    Vertice v2;
    Vertice v3;

    dadosVia = tratar_arquivo_via(dadosArquivoVia);

    TEST_ASSERT_NOT_NULL(dadosVia);

    grafo = obter_grafo_via(dadosVia);
    v1 = obter_vertice_via(dadosVia, "(b0|0,0)");
    v2 = obter_vertice_via(dadosVia, "(b0|0,0)(b0|0,1).0");
    v3 = obter_vertice_via(dadosVia, "(b0|0,1)");

    TEST_ASSERT_NOT_NULL(grafo);
    TEST_ASSERT_NOT_NULL(v1);
    TEST_ASSERT_NOT_NULL(v2);
    TEST_ASSERT_NOT_NULL(v3);
    TEST_ASSERT_EQUAL_STRING("(b0|0,0)", getIdVertice(v1));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 10.0f, getXVertice(v1));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 10.0f, getYVertice(v1));
    TEST_ASSERT_NULL(obter_vertice_via(dadosVia, "inexistente"));
}

void test_tratar_arquivo_via_deve_criar_arestas_com_atributos(void) {
    Grafo grafo;
    Vertice v1;
    Vertice v2;
    Vertice v3;
    Aresta aresta12;
    Aresta aresta23;

    dadosVia = tratar_arquivo_via(dadosArquivoVia);
    TEST_ASSERT_NOT_NULL(dadosVia);

    grafo = obter_grafo_via(dadosVia);
    v1 = obter_vertice_via(dadosVia, "(b0|0,0)");
    v2 = obter_vertice_via(dadosVia, "(b0|0,0)(b0|0,1).0");
    v3 = obter_vertice_via(dadosVia, "(b0|0,1)");

    TEST_ASSERT_TRUE(arestaExiste(grafo, v1, v2));
    TEST_ASSERT_TRUE(arestaExiste(grafo, v2, v3));
    TEST_ASSERT_FALSE(arestaExiste(grafo, v3, v2));

    aresta12 = insereAresta(grafo, v1, v2);
    aresta23 = insereAresta(grafo, v2, v3);

    TEST_ASSERT_EQUAL_STRING("Al_Scoma_Way", getNomeAresta(aresta12));
    TEST_ASSERT_EQUAL_STRING("b01.1", getLdirAresta(aresta12));
    TEST_ASSERT_EQUAL_STRING("#", getLesqAresta(aresta12));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 67.5f, getComprimentoAresta(aresta12));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 3.841354f, getVelocidadeMediaAresta(aresta12));

    TEST_ASSERT_EQUAL_STRING("Al_Scoma_Way", getNomeAresta(aresta23));
    TEST_ASSERT_EQUAL_STRING("b01.1", getLdirAresta(aresta23));
    TEST_ASSERT_EQUAL_STRING("#", getLesqAresta(aresta23));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 67.5f, getComprimentoAresta(aresta23));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 3.841354f, getVelocidadeMediaAresta(aresta23));
}

void test_tratar_arquivo_via_deve_retornar_null_para_aresta_com_vertice_inexistente(void) {
    FILE *arquivo;

    destruir_dados_arquivo(dadosArquivoVia);
    dadosArquivoVia = NULL;

    arquivo = fopen(ARQUIVO_VIA, "w");
    TEST_ASSERT_NOT_NULL(arquivo);
    fputs("1\n", arquivo);
    fputs("v v1 10 10\n", arquivo);
    fputs("e v1 v2 cepA cepB 100 5 Rua_A\n", arquivo);
    fclose(arquivo);

    dadosArquivoVia = criar_dados_arquivo((char *)ARQUIVO_VIA);
    TEST_ASSERT_NOT_NULL(dadosArquivoVia);

    dadosVia = tratar_arquivo_via(dadosArquivoVia);

    TEST_ASSERT_NULL(dadosVia);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_tratar_arquivo_via_deve_criar_grafo_e_vertices_indexados);
    RUN_TEST(test_tratar_arquivo_via_deve_criar_arestas_com_atributos);
    RUN_TEST(test_tratar_arquivo_via_deve_retornar_null_para_aresta_com_vertice_inexistente);

    return UNITY_END();
}
