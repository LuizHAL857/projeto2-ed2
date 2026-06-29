#include "../unity/unity.h"

#include "../include/grafo_direcionado.h"
#include "../include/hash_table.h"
#include "../include/leitor_arquivos.h"
#include "../include/quadra.h"
#include "../include/trata_qry.h"
#include "../include/trata_via.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *ARQUIVO_GEO = "cidade.geo";
static const char *ARQUIVO_VIA = "cidade.via";
static const char *ARQUIVO_QRY = "consultas.qry";
static const char *ARQUIVO_SVG = "cidade-consultas.svg";
static const char *ARQUIVO_TXT = "cidade-consultas.txt";

static DadosDoArquivo dadosGeoArquivo;
static DadosDoArquivo dadosViaArquivo;
static DadosDoArquivo dadosQryArquivo;
static HashTable quadras;
static DadosVia dadosVia;
static DadosQry dadosQry;

static void escreveArquivo(const char *caminho, const char *conteudo) {
    FILE *arquivo = fopen(caminho, "w");

    TEST_ASSERT_NOT_NULL(arquivo);
    TEST_ASSERT_NOT_EQUAL(EOF, fputs(conteudo, arquivo));
    TEST_ASSERT_EQUAL_INT(0, fclose(arquivo));
}

static char *leArquivoCompleto(const char *caminho) {
    FILE *arquivo = fopen(caminho, "rb");
    char *conteudo;
    long tamanho;

    if (arquivo == NULL) {
        return NULL;
    }

    if (fseek(arquivo, 0, SEEK_END) != 0) {
        fclose(arquivo);
        return NULL;
    }

    tamanho = ftell(arquivo);
    if (tamanho < 0 || fseek(arquivo, 0, SEEK_SET) != 0) {
        fclose(arquivo);
        return NULL;
    }

    conteudo = malloc((size_t)tamanho + 1);
    if (conteudo == NULL) {
        fclose(arquivo);
        return NULL;
    }

    if (fread(conteudo, 1, (size_t)tamanho, arquivo) != (size_t)tamanho) {
        free(conteudo);
        fclose(arquivo);
        return NULL;
    }

    conteudo[tamanho] = '\0';
    fclose(arquivo);
    return conteudo;
}

static void carregaQry(const char *conteudo) {
    destruir_dados_arquivo(dadosQryArquivo);
    dadosQryArquivo = NULL;

    escreveArquivo(ARQUIVO_QRY, conteudo);
    dadosQryArquivo = criar_dados_arquivo((char *)ARQUIVO_QRY);
    TEST_ASSERT_NOT_NULL(dadosQryArquivo);
}

static Aresta obtemAresta(const char *origemId, const char *destinoId) {
    Grafo grafo = obter_grafo_via(dadosVia);
    Vertice origem = obter_vertice_via(dadosVia, origemId);
    Vertice destino = obter_vertice_via(dadosVia, destinoId);

    TEST_ASSERT_NOT_NULL(grafo);
    TEST_ASSERT_NOT_NULL(origem);
    TEST_ASSERT_NOT_NULL(destino);
    TEST_ASSERT_TRUE(arestaExiste(grafo, origem, destino));
    return insereAresta(grafo, origem, destino);
}

void setUp(void) {
    Quadra quadra;

    dadosGeoArquivo = NULL;
    dadosViaArquivo = NULL;
    dadosQryArquivo = NULL;
    quadras = NULL;
    dadosVia = NULL;
    dadosQry = NULL;

    escreveArquivo(ARQUIVO_GEO, "q cep1 10 20 100 60\n");
    escreveArquivo(
        ARQUIVO_VIA,
        "4\n"
        "v v1 35 20\n"
        "v v2 80 20\n"
        "v v3 100 100\n"
        "v v4 110 100\n"
        "e v1 v2 cep1 # 45 5 Rua_A\n"
        "e v2 v3 cep1 # 90 8 Rua_B\n"
        "e v3 v4 cep1 # 10 2 Rua_C\n"
    );
    escreveArquivo(ARQUIVO_QRY, "@o? R0 cep1 S 25\n");

    dadosGeoArquivo = criar_dados_arquivo((char *)ARQUIVO_GEO);
    dadosViaArquivo = criar_dados_arquivo((char *)ARQUIVO_VIA);
    dadosQryArquivo = criar_dados_arquivo((char *)ARQUIVO_QRY);

    TEST_ASSERT_NOT_NULL(dadosGeoArquivo);
    TEST_ASSERT_NOT_NULL(dadosViaArquivo);
    TEST_ASSERT_NOT_NULL(dadosQryArquivo);

    quadras = criaHashTable(17);
    TEST_ASSERT_NOT_NULL(quadras);

    quadra = criaQuadra("cep1", 10.0f, 20.0f, 100.0f, 60.0f);
    TEST_ASSERT_NOT_NULL(quadra);
    TEST_ASSERT_TRUE(setEstiloQuadra(quadra, 1.0f, "orange", "brown"));
    TEST_ASSERT_TRUE(insereHashTable(quadras, "cep1", quadra));

    dadosVia = tratar_arquivo_via(dadosViaArquivo);
    TEST_ASSERT_NOT_NULL(dadosVia);
}

void tearDown(void) {
    desaloca_qry(dadosQry);
    desaloca_via(dadosVia);
    liberaHashTable(quadras, liberaQuadra);
    destruir_dados_arquivo(dadosQryArquivo);
    destruir_dados_arquivo(dadosViaArquivo);
    destruir_dados_arquivo(dadosGeoArquivo);

    remove(ARQUIVO_TXT);
    remove(ARQUIVO_SVG);
    remove(ARQUIVO_QRY);
    remove(ARQUIVO_VIA);
    remove(ARQUIVO_GEO);
}

void test_comando_o_deve_armazenar_endereco_e_gerar_as_saidas(void) {
    float x;
    float y;
    char *svg;
    char *txt;

    carregaQry("@o? R3 cep1 S 25\n");
    dadosQry = tratar_arquivo_qry(dadosQryArquivo, dadosGeoArquivo,
                                  quadras, dadosVia, ".");

    TEST_ASSERT_NOT_NULL(dadosQry);
    TEST_ASSERT_TRUE(obter_registrador_qry(dadosQry, 3, &x, &y));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 35.0f, x);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 20.0f, y);
    TEST_ASSERT_FALSE(obter_registrador_qry(dadosQry, 2, &x, &y));
    TEST_ASSERT_FALSE(obter_registrador_qry(dadosQry, 11, &x, &y));

    svg = leArquivoCompleto(ARQUIVO_SVG);
    txt = leArquivoCompleto(ARQUIVO_TXT);
    TEST_ASSERT_NOT_NULL(svg);
    TEST_ASSERT_NOT_NULL(txt);
    TEST_ASSERT_NOT_NULL(strstr(svg, "stroke-dasharray"));
    TEST_ASSERT_NOT_NULL(strstr(svg, "red"));
    TEST_ASSERT_NOT_NULL(strstr(svg, "R3"));
    TEST_ASSERT_NOT_NULL(strstr(txt, "R3"));
    TEST_ASSERT_NOT_NULL(strstr(txt, "35"));
    TEST_ASSERT_NOT_NULL(strstr(txt, "20"));

    free(svg);
    free(txt);
}

void test_comando_o_deve_calcular_as_quatro_faces_da_quadra(void) {
    float x;
    float y;

    carregaQry(
        "@o? R0 cep1 S 25\n"
        "@o? R1 cep1 N 25\n"
        "@o? R2 cep1 L 25\n"
        "@o? R3 cep1 O 25\n"
    );
    dadosQry = tratar_arquivo_qry(dadosQryArquivo, dadosGeoArquivo,
                                  quadras, dadosVia, ".");

    TEST_ASSERT_NOT_NULL(dadosQry);

    TEST_ASSERT_TRUE(obter_registrador_qry(dadosQry, 0, &x, &y));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 35.0f, x);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 20.0f, y);

    TEST_ASSERT_TRUE(obter_registrador_qry(dadosQry, 1, &x, &y));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 35.0f, x);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 80.0f, y);

    TEST_ASSERT_TRUE(obter_registrador_qry(dadosQry, 2, &x, &y));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 10.0f, x);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 45.0f, y);

    TEST_ASSERT_TRUE(obter_registrador_qry(dadosQry, 3, &x, &y));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 110.0f, x);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 45.0f, y);
}

void test_comando_mvm_deve_alterar_apenas_arestas_contidas_na_regiao(void) {
    Aresta interna = obtemAresta("v1", "v2");
    Aresta parcialmenteExterna = obtemAresta("v2", "v3");

    carregaQry("mvm 12 30 15 60 20\n");
    dadosQry = tratar_arquivo_qry(dadosQryArquivo, dadosGeoArquivo,
                                  quadras, dadosVia, ".");

    TEST_ASSERT_NOT_NULL(dadosQry);
    TEST_ASSERT_FLOAT_WITHIN(
        0.001f, 12.0f, getVelocidadeMediaAresta(interna)
    );
    TEST_ASSERT_FLOAT_WITHIN(
        0.001f, 8.0f, getVelocidadeMediaAresta(parcialmenteExterna)
    );
}

void test_comando_regs_deve_reportar_componentes_e_desenhar_limites(void) {
    char *svg;
    char *txt;

    carregaQry("regs 6\n");
    dadosQry = tratar_arquivo_qry(dadosQryArquivo, dadosGeoArquivo,
                                  quadras, dadosVia, ".");

    TEST_ASSERT_NOT_NULL(dadosQry);
    svg = leArquivoCompleto(ARQUIVO_SVG);
    txt = leArquivoCompleto(ARQUIVO_TXT);
    TEST_ASSERT_NOT_NULL(svg);
    TEST_ASSERT_NOT_NULL(txt);
    TEST_ASSERT_NOT_NULL(strstr(svg, "opacity"));
    TEST_ASSERT_NOT_NULL(strstr(txt, "componente"));

    free(svg);
    free(txt);
}

void test_comando_exp_deve_aumentar_velocidade_e_destacar_arestas(void) {
    Aresta lentaA = obtemAresta("v1", "v2");
    Aresta rapida = obtemAresta("v2", "v3");
    Aresta lentaB = obtemAresta("v3", "v4");
    char *svg;

    carregaQry("exp 6\n");
    dadosQry = tratar_arquivo_qry(dadosQryArquivo, dadosGeoArquivo,
                                  quadras, dadosVia, ".");

    TEST_ASSERT_NOT_NULL(dadosQry);
    TEST_ASSERT_FLOAT_WITHIN(
        0.001f, 7.5f, getVelocidadeMediaAresta(lentaA)
    );
    TEST_ASSERT_FLOAT_WITHIN(
        0.001f, 8.0f, getVelocidadeMediaAresta(rapida)
    );
    TEST_ASSERT_FLOAT_WITHIN(
        0.001f, 3.0f, getVelocidadeMediaAresta(lentaB)
    );

    svg = leArquivoCompleto(ARQUIVO_SVG);
    TEST_ASSERT_NOT_NULL(svg);
    TEST_ASSERT_NOT_NULL(strstr(svg, "red"));
    TEST_ASSERT_NOT_NULL(strstr(svg, "stroke-width"));
    free(svg);
}

void test_comando_p_deve_descrever_e_animar_os_dois_percursos(void) {
    char *svg;
    char *txt;

    carregaQry(
        "@o? R0 cep1 S 25\n"
        "@o? R1 cep1 S 70\n"
        "p? R0 R1 blue green\n"
    );
    dadosQry = tratar_arquivo_qry(dadosQryArquivo, dadosGeoArquivo,
                                  quadras, dadosVia, ".");

    TEST_ASSERT_NOT_NULL(dadosQry);
    svg = leArquivoCompleto(ARQUIVO_SVG);
    txt = leArquivoCompleto(ARQUIVO_TXT);
    TEST_ASSERT_NOT_NULL(svg);
    TEST_ASSERT_NOT_NULL(txt);
    TEST_ASSERT_NOT_NULL(strstr(svg, "blue"));
    TEST_ASSERT_NOT_NULL(strstr(svg, "green"));
    TEST_ASSERT_NOT_NULL(strstr(svg, "animateMotion"));
    TEST_ASSERT_NOT_NULL(strstr(svg, ">I<"));
    TEST_ASSERT_NOT_NULL(strstr(svg, ">F<"));
    TEST_ASSERT_NOT_NULL(strstr(txt, "Rua_A"));

    free(svg);
    free(txt);
}

void test_comando_invalido_deve_falhar_sem_manter_arquivos_parciais(void) {
    carregaQry("invalido 10 20\n");
    dadosQry = tratar_arquivo_qry(dadosQryArquivo, dadosGeoArquivo,
                                  quadras, dadosVia, ".");

    TEST_ASSERT_NULL(dadosQry);
    TEST_ASSERT_NULL(leArquivoCompleto(ARQUIVO_SVG));
    TEST_ASSERT_NULL(leArquivoCompleto(ARQUIVO_TXT));
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_comando_o_deve_armazenar_endereco_e_gerar_as_saidas);
    RUN_TEST(test_comando_o_deve_calcular_as_quatro_faces_da_quadra);
#ifdef TRATA_QRY_COMPLETO
    RUN_TEST(test_comando_mvm_deve_alterar_apenas_arestas_contidas_na_regiao);
    RUN_TEST(test_comando_regs_deve_reportar_componentes_e_desenhar_limites);
    RUN_TEST(test_comando_exp_deve_aumentar_velocidade_e_destacar_arestas);
    RUN_TEST(test_comando_p_deve_descrever_e_animar_os_dois_percursos);
#endif
    RUN_TEST(test_comando_invalido_deve_falhar_sem_manter_arquivos_parciais);

    return UNITY_END();
}
