#include "../unity/unity.h"
#include "../include/quadra.h"

#include <string.h>

static Quadra quadra;

void setUp(void) {
    quadra = criaQuadra("cep15", 10.0f, 20.0f, 100.0f, 60.0f);
}

void tearDown(void) {
    liberaQuadra(quadra);
    quadra = NULL;
}

void test_criaQuadra_deve_preservar_cep_e_geometria(void) {
    TEST_ASSERT_NOT_NULL(quadra);
    TEST_ASSERT_EQUAL_STRING("cep15", getCepQuadra(quadra));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 10.0f, getXQuadra(quadra));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 20.0f, getYQuadra(quadra));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 100.0f, getLarguraQuadra(quadra));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 60.0f, getAlturaQuadra(quadra));
}

void test_criaQuadra_deve_copiar_o_cep(void) {
    char cep[] = "cep20";
    Quadra outra = criaQuadra(cep, 1.0f, 2.0f, 3.0f, 4.0f);

    TEST_ASSERT_NOT_NULL(outra);
    strcpy(cep, "outro");

    TEST_ASSERT_EQUAL_STRING("cep20", getCepQuadra(outra));

    liberaQuadra(outra);
}

void test_criaQuadra_deve_rejeitar_parametros_invalidos(void) {
    TEST_ASSERT_NULL(criaQuadra(NULL, 0.0f, 0.0f, 10.0f, 10.0f));
    TEST_ASSERT_NULL(criaQuadra("", 0.0f, 0.0f, 10.0f, 10.0f));
    TEST_ASSERT_NULL(criaQuadra("cep1", 0.0f, 0.0f, 0.0f, 10.0f));
    TEST_ASSERT_NULL(criaQuadra("cep1", 0.0f, 0.0f, 10.0f, -1.0f));
}

void test_setEstiloQuadra_deve_definir_espessura_e_cores(void) {
    TEST_ASSERT_TRUE(setEstiloQuadra(quadra, 2.5f, "lightblue", "black"));

    TEST_ASSERT_FLOAT_WITHIN(
        0.001f, 2.5f, getEspessuraBordaQuadra(quadra)
    );
    TEST_ASSERT_EQUAL_STRING(
        "lightblue", getCorPreenchimentoQuadra(quadra)
    );
    TEST_ASSERT_EQUAL_STRING("black", getCorBordaQuadra(quadra));
}

void test_setEstiloQuadra_deve_copiar_as_cores(void) {
    char preenchimento[] = "yellow";
    char borda[] = "blue";

    TEST_ASSERT_TRUE(
        setEstiloQuadra(quadra, 1.0f, preenchimento, borda)
    );

    strcpy(preenchimento, "orange");
    strcpy(borda, "red");

    TEST_ASSERT_EQUAL_STRING(
        "yellow", getCorPreenchimentoQuadra(quadra)
    );
    TEST_ASSERT_EQUAL_STRING("blue", getCorBordaQuadra(quadra));
}

void test_setEstiloQuadra_deve_substituir_estilo_anterior(void) {
    TEST_ASSERT_TRUE(setEstiloQuadra(quadra, 1.0f, "white", "gray"));
    TEST_ASSERT_TRUE(setEstiloQuadra(quadra, 3.0f, "green", "navy"));

    TEST_ASSERT_FLOAT_WITHIN(
        0.001f, 3.0f, getEspessuraBordaQuadra(quadra)
    );
    TEST_ASSERT_EQUAL_STRING(
        "green", getCorPreenchimentoQuadra(quadra)
    );
    TEST_ASSERT_EQUAL_STRING("navy", getCorBordaQuadra(quadra));
}

void test_setEstiloQuadra_deve_rejeitar_parametros_invalidos(void) {
    TEST_ASSERT_FALSE(setEstiloQuadra(NULL, 1.0f, "white", "black"));
    TEST_ASSERT_FALSE(setEstiloQuadra(quadra, -1.0f, "white", "black"));
    TEST_ASSERT_FALSE(setEstiloQuadra(quadra, 1.0f, NULL, "black"));
    TEST_ASSERT_FALSE(setEstiloQuadra(quadra, 1.0f, "white", NULL));
}

void test_getters_devem_retornar_valores_seguros_para_quadra_null(void) {
    TEST_ASSERT_NULL(getCepQuadra(NULL));
    TEST_ASSERT_NULL(getCorPreenchimentoQuadra(NULL));
    TEST_ASSERT_NULL(getCorBordaQuadra(NULL));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, getXQuadra(NULL));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, getYQuadra(NULL));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, getLarguraQuadra(NULL));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, getAlturaQuadra(NULL));
    TEST_ASSERT_FLOAT_WITHIN(
        0.001f, 0.0f, getEspessuraBordaQuadra(NULL)
    );
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_criaQuadra_deve_preservar_cep_e_geometria);
    RUN_TEST(test_criaQuadra_deve_copiar_o_cep);
    RUN_TEST(test_criaQuadra_deve_rejeitar_parametros_invalidos);
    RUN_TEST(test_setEstiloQuadra_deve_definir_espessura_e_cores);
    RUN_TEST(test_setEstiloQuadra_deve_copiar_as_cores);
    RUN_TEST(test_setEstiloQuadra_deve_substituir_estilo_anterior);
    RUN_TEST(test_setEstiloQuadra_deve_rejeitar_parametros_invalidos);
    RUN_TEST(test_getters_devem_retornar_valores_seguros_para_quadra_null);

    return UNITY_END();
}
