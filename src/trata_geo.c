#include "../include/trata_geo.h"

#include "../include/quadra.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAMANHO_CEP 128
#define TAMANHO_COR 128

typedef struct {
    float espessuraBorda;
    char corPreenchimento[TAMANHO_COR];
    char corBorda[TAMANHO_COR];
} EstiloQuadra;

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

static bool processaComandoCq(const char *linha, EstiloQuadra *estilo) {
    float espessura;
    char preenchimento[TAMANHO_COR];
    char borda[TAMANHO_COR];

    if (sscanf(linha, " cq %f %127s %127s",
               &espessura, preenchimento, borda) != 3 ||
        espessura < 0.0f) {
        return false;
    }

    estilo->espessuraBorda = espessura;
    strcpy(estilo->corPreenchimento, preenchimento);
    strcpy(estilo->corBorda, borda);
    return true;
}

static bool processaComandoQ(const char *linha, const EstiloQuadra *estilo,
                             HashTable quadras) {
    char cep[TAMANHO_CEP];
    float x;
    float y;
    float largura;
    float altura;
    Quadra quadra;

    if (sscanf(linha, " q %127s %f %f %f %f",
               cep, &x, &y, &largura, &altura) != 5 ||
        chaveExisteHashTable(quadras, cep)) {
        return false;
    }

    quadra = criaQuadra(cep, x, y, largura, altura);
    if (quadra == NULL) {
        return false;
    }

    if (!setEstiloQuadra(quadra, estilo->espessuraBorda,
                         estilo->corPreenchimento, estilo->corBorda) ||
        !insereHashTable(quadras, cep, quadra)) {
        liberaQuadra(quadra);
        return false;
    }

    return true;
}

static bool processaLinhaGeo(const char *linha, EstiloQuadra *estilo,
                             HashTable quadras) {
    char comando[3];

    if (sscanf(linha, " %2s", comando) != 1) {
        return false;
    }

    if (strcmp(comando, "cq") == 0) {
        return processaComandoCq(linha, estilo);
    }

    if (strcmp(comando, "q") == 0) {
        return processaComandoQ(linha, estilo, quadras);
    }

    return false;
}

static void atualizaLimites(const char *chave, void *valor, void *extra) {
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
                "  <rect x=\"%.2f\" y=\"%.2f\" width=\"%.2f\" "
                "height=\"%.2f\" fill=\"%s\" stroke=\"%s\" "
                "stroke-width=\"%.2f\" />\n"
                "  <text x=\"%.2f\" y=\"%.2f\" font-size=\"10\" "
                "fill=\"black\">%s</text>\n",
                getXQuadra(quadra), getYQuadra(quadra),
                getLarguraQuadra(quadra), getAlturaQuadra(quadra),
                getCorPreenchimentoQuadra(quadra),
                getCorBordaQuadra(quadra),
                getEspessuraBordaQuadra(quadra),
                getXQuadra(quadra) + 2.0f,
                getYQuadra(quadra) + 12.0f,
                getCepQuadra(quadra)) < 0) {
        contexto->sucesso = false;
    }
}

static bool geraSvgInicial(HashTable quadras, const char *caminhoSvg) {
    FILE *arquivo;
    LimitesSvg limites = {false, 0.0f, 0.0f, 1.0f, 1.0f};
    ContextoSvg contexto;
    float largura;
    float altura;

    percorreHashTable(quadras, atualizaLimites, &limites);

    if (!limites.inicializado) {
        limites.minX = 0.0f;
        limites.minY = 0.0f;
        limites.maxX = 1.0f;
        limites.maxY = 1.0f;
    }

    largura = limites.maxX - limites.minX;
    altura = limites.maxY - limites.minY;

    arquivo = fopen(caminhoSvg, "w");
    if (arquivo == NULL) {
        return false;
    }

    contexto.arquivo = arquivo;
    contexto.sucesso =
        fprintf(arquivo,
                "<svg xmlns=\"http://www.w3.org/2000/svg\" "
                "viewBox=\"%.2f %.2f %.2f %.2f\">\n",
                limites.minX, limites.minY, largura, altura) >= 0;

    percorreHashTable(quadras, escreveQuadraSvg, &contexto);

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

HashTable tratar_arquivo_geo(DadosDoArquivo dadosGeo,
                             const char *diretorioSaida) {
    Lista linhas;
    Celula atual;
    HashTable quadras;
    EstiloQuadra estilo = {1.0f, "none", "black"};
    char *nomeBase;
    char *caminhoSvg;
    int capacidade;

    if (dadosGeo == NULL || diretorioSaida == NULL ||
        !arquivo_termina_com(obter_nome_arquivo(dadosGeo), ".geo")) {
        return NULL;
    }

    linhas = obter_lista_linhas(dadosGeo);
    if (linhas == NULL) {
        return NULL;
    }

    capacidade = getTamanhoLista(linhas) * 2 + 1;
    if (capacidade < 17) {
        capacidade = 17;
    }

    quadras = criaHashTable(capacidade);
    if (quadras == NULL) {
        return NULL;
    }

    atual = getInicioLista(linhas);
    while (atual != NULL) {
        const char *linha = getConteudoCelula(atual);

        if (!processaLinhaGeo(linha, &estilo, quadras)) {
            desaloca_geo(quadras);
            return NULL;
        }

        atual = getProxCelula(atual);
    }

    nomeBase = arquivo_extrair_nome_base(obter_nome_arquivo(dadosGeo), ".geo");
    if (nomeBase == NULL) {
        desaloca_geo(quadras);
        return NULL;
    }

    caminhoSvg = arquivo_montar_caminho_saida(
        diretorioSaida, nomeBase, ".svg"
    );
    free(nomeBase);

    if (caminhoSvg == NULL || !geraSvgInicial(quadras, caminhoSvg)) {
        free(caminhoSvg);
        desaloca_geo(quadras);
        return NULL;
    }

    free(caminhoSvg);
    return quadras;
}

void desaloca_geo(HashTable quadras) {
    liberaHashTable(quadras, liberaQuadra);
}
