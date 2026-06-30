#include "../include/trata_qry.h"

#include "../include/quadra.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define QUANTIDADE_REGISTRADORES 11
#define TAMANHO_CEP 128
#define MARGEM_SUPERIOR_SVG 30.0f
#define MARGEM_LATERAL_SVG 20.0f
#define MARGEM_INFERIOR_SVG 20.0f

typedef struct {
    bool definido;
    float x;
    float y;
} RegistradorQry;

typedef struct {
    int indiceRegistrador;
    char cep[TAMANHO_CEP];
    char face;
    float numero;
    float x;
    float y;
} MarcacaoEndereco;

typedef struct {
    RegistradorQry registradores[QUANTIDADE_REGISTRADORES];
    MarcacaoEndereco *marcacoes;
    int quantidadeMarcacoes;
    int capacidadeMarcacoes;
} stDadosQry;

typedef struct {
    bool inicializado;
    float minX;
    float minY;
    float maxX;
    float maxY;
} LimitesSvg;

typedef struct {
    FILE *arquivo;
    bool sucesso;
} ContextoSvg;

static bool leIndiceRegistrador(const char *texto, int *indice) {
    char prefixo;
    char sobra;
    int valor;

    if (texto == NULL || indice == NULL ||
        sscanf(texto, "%c%d%c", &prefixo, &valor, &sobra) != 2 ||
        prefixo != 'R' || valor < 0 || valor >= QUANTIDADE_REGISTRADORES) {
        return false;
    }

    *indice = valor;
    return true;
}

static bool calculaCoordenadaEndereco(Quadra quadra, char face, float numero,
                                      float *x, float *y) {
    float limite;

    if (quadra == NULL || x == NULL || y == NULL || numero < 0.0f) {
        return false;
    }

    if (face == 'S' || face == 'N') {
        limite = getLarguraQuadra(quadra);
    } else if (face == 'L' || face == 'O') {
        limite = getAlturaQuadra(quadra);
    } else {
        return false;
    }

    if (numero > limite) {
        return false;
    }

    switch (face) {
        case 'S':
            *x = getXQuadra(quadra) + numero;
            *y = getYQuadra(quadra);
            break;
        case 'N':
            *x = getXQuadra(quadra) + numero;
            *y = getYQuadra(quadra) + getAlturaQuadra(quadra);
            break;
        case 'L':
            *x = getXQuadra(quadra);
            *y = getYQuadra(quadra) + numero;
            break;
        case 'O':
            *x = getXQuadra(quadra) + getLarguraQuadra(quadra);
            *y = getYQuadra(quadra) + numero;
            break;
        default:
            return false;
    }

    return true;
}

static bool adicionaMarcacao(stDadosQry *dados, int indiceRegistrador,
                             const char *cep, char face, float numero,
                             float x, float y) {
    MarcacaoEndereco *novasMarcacoes;
    MarcacaoEndereco *marcacao;
    int novaCapacidade;

    if (dados->quantidadeMarcacoes == dados->capacidadeMarcacoes) {
        novaCapacidade = dados->capacidadeMarcacoes == 0
            ? 4
            : dados->capacidadeMarcacoes * 2;
        novasMarcacoes = realloc(
            dados->marcacoes,
            (size_t)novaCapacidade * sizeof(MarcacaoEndereco)
        );
        if (novasMarcacoes == NULL) {
            return false;
        }

        dados->marcacoes = novasMarcacoes;
        dados->capacidadeMarcacoes = novaCapacidade;
    }

    marcacao = &dados->marcacoes[dados->quantidadeMarcacoes];
    marcacao->indiceRegistrador = indiceRegistrador;
    strcpy(marcacao->cep, cep);
    marcacao->face = face;
    marcacao->numero = numero;
    marcacao->x = x;
    marcacao->y = y;
    dados->quantidadeMarcacoes++;
    return true;
}

static bool processaComandoO(const char *linha, HashTable quadras,
                             stDadosQry *dados) {
    char registrador[16];
    char cep[TAMANHO_CEP];
    char faceTexto[2];
    int indiceRegistrador;
    float numero;
    float x;
    float y;
    Quadra quadra;

    if (sscanf(linha, " @o? %15s %127s %1s %f",
               registrador, cep, faceTexto, &numero) != 4 ||
        !leIndiceRegistrador(registrador, &indiceRegistrador)) {
        return false;
    }

    quadra = buscaHashTable(quadras, cep);
    if (!calculaCoordenadaEndereco(quadra, faceTexto[0], numero, &x, &y) ||
        !adicionaMarcacao(dados, indiceRegistrador, cep, faceTexto[0],
                          numero, x, y)) {
        return false;
    }

    dados->registradores[indiceRegistrador].definido = true;
    dados->registradores[indiceRegistrador].x = x;
    dados->registradores[indiceRegistrador].y = y;
    return true;
}

static bool processaLinhaQry(const char *linha, HashTable quadras,
                             stDadosQry *dados) {
    char comando[8];

    if (sscanf(linha, " %7s", comando) != 1) {
        return false;
    }

    if (strcmp(comando, "@o?") == 0) {
        return processaComandoO(linha, quadras, dados);
    }

    return false;
}

static void atualizaLimitesSvg(const char *chave, void *valor, void *extra) {
    Quadra quadra = valor;
    LimitesSvg *limites = extra;
    float x;
    float y;
    float maxX;
    float maxY;

    (void)chave;

    if (quadra == NULL || limites == NULL) {
        return;
    }

    x = getXQuadra(quadra);
    y = getYQuadra(quadra);
    maxX = x + getLarguraQuadra(quadra);
    maxY = y + getAlturaQuadra(quadra);

    if (!limites->inicializado) {
        limites->minX = x;
        limites->minY = y;
        limites->maxX = maxX;
        limites->maxY = maxY;
        limites->inicializado = true;
        return;
    }

    if (x < limites->minX) {
        limites->minX = x;
    }
    if (y < limites->minY) {
        limites->minY = y;
    }
    if (maxX > limites->maxX) {
        limites->maxX = maxX;
    }
    if (maxY > limites->maxY) {
        limites->maxY = maxY;
    }
}

static void escreveQuadraSvg(const char *chave, void *valor, void *extra) {
    Quadra quadra = valor;
    ContextoSvg *contexto = extra;

    (void)chave;

    if (quadra == NULL || contexto == NULL || !contexto->sucesso) {
        return;
    }

    if (fprintf(contexto->arquivo,
                "  <rect id=\"%s\" x=\"%.2f\" y=\"%.2f\" width=\"%.2f\" "
                "height=\"%.2f\" fill=\"%s\" stroke=\"%s\" "
                "fill-opacity=\"0.8\" stroke-width=\"%.2f\" />\n"
                "  <text x=\"%.2f\" y=\"%.2f\" fill=\"%s\" "
                "stroke=\"black\" font-size=\"12\">%s</text>\n",
                getCepQuadra(quadra),
                getXQuadra(quadra), getYQuadra(quadra),
                getLarguraQuadra(quadra), getAlturaQuadra(quadra),
                getCorPreenchimentoQuadra(quadra),
                getCorBordaQuadra(quadra),
                getEspessuraBordaQuadra(quadra),
                getXQuadra(quadra) + 5.0f,
                getYQuadra(quadra) + 9.0f,
                getCorBordaQuadra(quadra),
                getCepQuadra(quadra)) < 0) {
        contexto->sucesso = false;
    }
}

static bool escreveMarcacoesSvg(FILE *arquivo, const stDadosQry *dados,
                                float topo) {
    int i;

    for (i = 0; i < dados->quantidadeMarcacoes; i++) {
        const MarcacaoEndereco *marcacao = &dados->marcacoes[i];

        if (fprintf(arquivo,
                    "  <line x1=\"%.2f\" y1=\"%.2f\" x2=\"%.2f\" "
                    "y2=\"%.2f\" stroke=\"red\" stroke-width=\"1\" "
                    "stroke-dasharray=\"5,5\" />\n"
                    "  <text x=\"%.2f\" y=\"%.2f\" font-size=\"10\" "
                    "fill=\"red\">R%d</text>\n",
                    marcacao->x, marcacao->y, marcacao->x, topo,
                    marcacao->x + 2.0f, topo + 10.0f,
                    marcacao->indiceRegistrador) < 0) {
            return false;
        }
    }

    return true;
}

static bool geraSvgQry(HashTable quadras, const stDadosQry *dados,
                       const char *caminhoSvg) {
    LimitesSvg limites = {false, 0.0f, 0.0f, 1.0f, 1.0f};
    ContextoSvg contexto;
    FILE *arquivo;
    float topo;
    float largura;
    float altura;

    percorreHashTable(quadras, atualizaLimitesSvg, &limites);
    if (!limites.inicializado) {
        return false;
    }

    topo = limites.minY - MARGEM_SUPERIOR_SVG;
    largura = limites.maxX - limites.minX + 2.0f * MARGEM_LATERAL_SVG;
    altura = limites.maxY - topo + MARGEM_INFERIOR_SVG;

    arquivo = fopen(caminhoSvg, "w");
    if (arquivo == NULL) {
        return false;
    }

    contexto.arquivo = arquivo;
    contexto.sucesso =
        fprintf(arquivo,
                "<svg xmlns=\"http://www.w3.org/2000/svg\" "
                "viewBox=\"%.2f %.2f %.2f %.2f\">\n",
                limites.minX - MARGEM_LATERAL_SVG,
                topo, largura, altura) >= 0;

    percorreHashTable(quadras, escreveQuadraSvg, &contexto);
    if (contexto.sucesso) {
        contexto.sucesso = escreveMarcacoesSvg(arquivo, dados, topo);
    }
    if (contexto.sucesso && fputs("</svg>\n", arquivo) == EOF) {
        contexto.sucesso = false;
    }
    if (fclose(arquivo) != 0) {
        contexto.sucesso = false;
    }

    if (!contexto.sucesso) {
        remove(caminhoSvg);
    }
    return contexto.sucesso;
}

static bool geraTxtQry(const stDadosQry *dados, const char *caminhoTxt) {
    FILE *arquivo;
    bool sucesso = true;
    int i;

    arquivo = fopen(caminhoTxt, "w");
    if (arquivo == NULL) {
        return false;
    }

    for (i = 0; i < dados->quantidadeMarcacoes; i++) {
        const MarcacaoEndereco *marcacao = &dados->marcacoes[i];

        if (fprintf(arquivo,
                    "@o? R%d %s %c %.2f\n"
                    "R%d: (%.2f, %.2f)\n",
                    marcacao->indiceRegistrador, marcacao->cep,
                    marcacao->face, marcacao->numero,
                    marcacao->indiceRegistrador,
                    marcacao->x, marcacao->y) < 0) {
            sucesso = false;
            break;
        }
    }

    if (fclose(arquivo) != 0) {
        sucesso = false;
    }
    if (!sucesso) {
        remove(caminhoTxt);
    }
    return sucesso;
}

static bool montaCaminhosSaida(DadosDoArquivo dadosQryArquivo,
                               DadosDoArquivo dadosGeoArquivo,
                               const char *diretorioSaida,
                               char **caminhoSvg, char **caminhoTxt) {
    char *baseGeo;
    char *baseQry;
    char *baseSaida;

    baseGeo = arquivo_extrair_nome_base(
        obter_nome_arquivo(dadosGeoArquivo), ".geo"
    );
    baseQry = arquivo_extrair_nome_base(
        obter_nome_arquivo(dadosQryArquivo), ".qry"
    );
    if (baseGeo == NULL || baseQry == NULL) {
        free(baseGeo);
        free(baseQry);
        return false;
    }

    baseSaida = arquivo_montar_nome_composto(baseGeo, "-", baseQry);
    free(baseGeo);
    free(baseQry);
    if (baseSaida == NULL) {
        return false;
    }

    *caminhoSvg = arquivo_montar_caminho_saida(
        diretorioSaida, baseSaida, ".svg"
    );
    *caminhoTxt = arquivo_montar_caminho_saida(
        diretorioSaida, baseSaida, ".txt"
    );
    free(baseSaida);
    return *caminhoSvg != NULL && *caminhoTxt != NULL;
}

DadosQry tratar_arquivo_qry(DadosDoArquivo dadosQryArquivo,
                            DadosDoArquivo dadosGeoArquivo,
                            HashTable quadras,
                            DadosVia dadosVia,
                            const char *diretorioSaida) {
    stDadosQry *dados;
    Lista linhas;
    Celula atual;
    char *caminhoSvg = NULL;
    char *caminhoTxt = NULL;

    (void)dadosVia;

    if (dadosQryArquivo == NULL || dadosGeoArquivo == NULL ||
        quadras == NULL || diretorioSaida == NULL ||
        !arquivo_termina_com(obter_nome_arquivo(dadosQryArquivo), ".qry") ||
        !arquivo_termina_com(obter_nome_arquivo(dadosGeoArquivo), ".geo")) {
        return NULL;
    }

    linhas = obter_lista_linhas(dadosQryArquivo);
    if (linhas == NULL ||
        !montaCaminhosSaida(dadosQryArquivo, dadosGeoArquivo,
                            diretorioSaida, &caminhoSvg, &caminhoTxt)) {
        free(caminhoSvg);
        free(caminhoTxt);
        return NULL;
    }

    remove(caminhoSvg);
    remove(caminhoTxt);

    dados = calloc(1, sizeof(stDadosQry));
    if (dados == NULL) {
        free(caminhoSvg);
        free(caminhoTxt);
        return NULL;
    }

    atual = getInicioLista(linhas);
    while (atual != NULL) {
        if (!processaLinhaQry(getConteudoCelula(atual), quadras, dados)) {
            desaloca_qry(dados);
            free(caminhoSvg);
            free(caminhoTxt);
            return NULL;
        }
        atual = getProxCelula(atual);
    }

    if (!geraTxtQry(dados, caminhoTxt) ||
        !geraSvgQry(quadras, dados, caminhoSvg)) {
        remove(caminhoSvg);
        remove(caminhoTxt);
        desaloca_qry(dados);
        free(caminhoSvg);
        free(caminhoTxt);
        return NULL;
    }

    free(caminhoSvg);
    free(caminhoTxt);
    return dados;
}

bool obter_registrador_qry(DadosQry dadosQry, int indiceRegistrador,
                           float *x, float *y) {
    stDadosQry *dados = dadosQry;

    if (dados == NULL || indiceRegistrador < 0 ||
        indiceRegistrador >= QUANTIDADE_REGISTRADORES ||
        x == NULL || y == NULL ||
        !dados->registradores[indiceRegistrador].definido) {
        return false;
    }

    *x = dados->registradores[indiceRegistrador].x;
    *y = dados->registradores[indiceRegistrador].y;
    return true;
}

void desaloca_qry(DadosQry dadosQry) {
    stDadosQry *dados = dadosQry;

    if (dados == NULL) {
        return;
    }

    free(dados->marcacoes);
    free(dados);
}
