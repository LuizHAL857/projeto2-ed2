#include "../unity/unity.h"
#include "../include/hash_table.h"
#include "../include/leitor_arquivos.h"
#include "../include/quadra.h"
#include "../include/trata_geo.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *ARQUIVO_GEO = "test_trata_geo.geo";
static const char *ARQUIVO_SVG = "test_trata_geo.svg";

static DadosDoArquivo dadosGeo;
static HashTable quadras;

static void escreveGeoValido(void) {
    FILE *arquivo = fopen(ARQUIVO_GEO, "w");

    TEST_ASSERT_NOT_NULL(arquivo);
    fputs("cq 2.5 yellow blue\n", arquivo);
    fputs("q cep1 10 20 100 60\n", arquivo);
    fputs("cq 1.0 green black\n", arquivo);
    fputs("q cep2 150 30 80 40\n", arquivo);
    fclose(arquivo);
}

static char *leArquivoCompleto(const char *caminho) {
    FILE *arquivo;
    long tamanho;
    char *conteudo;

    arquivo = fopen(caminho, "rb");
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

    conteudo = malloc((size_t)tamanho + 1u);
    if (conteudo == NULL) {
        fclose(arquivo);
        return NULL;
    }

    if (fread(conteudo, 1u, (size_t)tamanho, arquivo) != (size_t)tamanho) {
        free(conteudo);
        fclose(arquivo);
        return NULL;
    }

    conteudo[tamanho] = '\0';
    fclose(arquivo);
    return conteudo;
}

void setUp(void) {
    escreveGeoValido();
    dadosGeo = criar_dados_arquivo((char *)ARQUIVO_GEO);
    quadras = NULL;
}

void tearDown(void) {
    desaloca_geo(quadras);
    destruir_dados_arquivo(dadosGeo);
    quadras = NULL;
    dadosGeo = NULL;
    remove(ARQUIVO_GEO);
    remove(ARQUIVO_SVG);
}

void test_tratar_arquivo_geo_deve_criar_quadras_indexadas_por_cep(void) {
    Quadra cep1;
    Quadra cep2;

    quadras = tratar_arquivo_geo(dadosGeo, ".");

    TEST_ASSERT_NOT_NULL(quadras);
    TEST_ASSERT_EQUAL_INT(2, getTamanhoHashTable(quadras));

    cep1 = buscaHashTable(quadras, "cep1");
    cep2 = buscaHashTable(quadras, "cep2");

    TEST_ASSERT_NOT_NULL(cep1);
    TEST_ASSERT_NOT_NULL(cep2);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 10.0f, getXQuadra(cep1));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 20.0f, getYQuadra(cep1));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 100.0f, getLarguraQuadra(cep1));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 60.0f, getAlturaQuadra(cep1));
}

void test_comando_cq_deve_afetar_apenas_quadras_seguintes(void) {
    Quadra cep1;
    Quadra cep2;

    quadras = tratar_arquivo_geo(dadosGeo, ".");
    TEST_ASSERT_NOT_NULL(quadras);

    cep1 = buscaHashTable(quadras, "cep1");
    cep2 = buscaHashTable(quadras, "cep2");

    TEST_ASSERT_FLOAT_WITHIN(
        0.001f, 2.5f, getEspessuraBordaQuadra(cep1)
    );
    TEST_ASSERT_EQUAL_STRING("yellow", getCorPreenchimentoQuadra(cep1));
    TEST_ASSERT_EQUAL_STRING("blue", getCorBordaQuadra(cep1));

    TEST_ASSERT_FLOAT_WITHIN(
        0.001f, 1.0f, getEspessuraBordaQuadra(cep2)
    );
    TEST_ASSERT_EQUAL_STRING("green", getCorPreenchimentoQuadra(cep2));
    TEST_ASSERT_EQUAL_STRING("black", getCorBordaQuadra(cep2));
}

void test_tratar_arquivo_geo_deve_gerar_svg_com_quadras_e_ceps(void) {
    char *svg;

    quadras = tratar_arquivo_geo(dadosGeo, ".");
    TEST_ASSERT_NOT_NULL(quadras);

    svg = leArquivoCompleto(ARQUIVO_SVG);
    TEST_ASSERT_NOT_NULL(svg);

    TEST_ASSERT_NOT_NULL(strstr(svg, "<svg"));
    TEST_ASSERT_NOT_NULL(strstr(svg, "<rect"));
    TEST_ASSERT_NOT_NULL(strstr(svg, "fill=\"yellow\""));
    TEST_ASSERT_NOT_NULL(strstr(svg, "stroke=\"blue\""));
    TEST_ASSERT_NOT_NULL(strstr(svg, ">cep1</text>"));
    TEST_ASSERT_NOT_NULL(strstr(svg, "fill=\"green\""));
    TEST_ASSERT_NOT_NULL(strstr(svg, ">cep2</text>"));
    TEST_ASSERT_NOT_NULL(strstr(svg, "</svg>"));

    free(svg);
}

void test_tratar_arquivo_geo_deve_retornar_null_para_comando_invalido(void) {
    FILE *arquivo;

    destruir_dados_arquivo(dadosGeo);
    dadosGeo = NULL;

    arquivo = fopen(ARQUIVO_GEO, "w");
    TEST_ASSERT_NOT_NULL(arquivo);
    fputs("comando-invalido cep1 10 20 30 40\n", arquivo);
    fclose(arquivo);

    dadosGeo = criar_dados_arquivo((char *)ARQUIVO_GEO);
    TEST_ASSERT_NOT_NULL(dadosGeo);

    quadras = tratar_arquivo_geo(dadosGeo, ".");

    TEST_ASSERT_NULL(quadras);
    TEST_ASSERT_NULL(leArquivoCompleto(ARQUIVO_SVG));
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_tratar_arquivo_geo_deve_criar_quadras_indexadas_por_cep);
    RUN_TEST(test_comando_cq_deve_afetar_apenas_quadras_seguintes);
    RUN_TEST(test_tratar_arquivo_geo_deve_gerar_svg_com_quadras_e_ceps);
    RUN_TEST(test_tratar_arquivo_geo_deve_retornar_null_para_comando_invalido);

    return UNITY_END();
}
