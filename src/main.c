#include "../include/leitor_arquivos.h"
#include "../include/trata_argumentos.h"
#include "../include/trata_geo.h"
#include "../include/trata_via.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    
    char *diretorioEntrada;
    char *nomeArquivoGeo;
    char *nomeArquivoVia;
    char *diretorioSaida;
    char *caminhoGeo;
    char *caminhoVia;
    DadosDoArquivo dadosGeo;
    DadosDoArquivo dadosArquivoVia;
    HashTable quadras;
    DadosVia dadosVia;

    diretorioEntrada = obter_valor_opcao(argc, argv, "e");
    nomeArquivoGeo = obter_valor_opcao(argc, argv, "f");
    nomeArquivoVia = obter_valor_opcao(argc, argv, "v");
    diretorioSaida = obter_valor_opcao(argc, argv, "o");
    caminhoVia = NULL;
    dadosArquivoVia = NULL;
    dadosVia = NULL;

    if (nomeArquivoGeo == NULL || diretorioSaida == NULL) {
        fprintf(stderr, "Erro: as opcoes -f e -o sao obrigatorias.\n");
        
        return 1;
    }

    caminhoGeo = montar_caminho_entrada(diretorioEntrada, nomeArquivoGeo);
    if (caminhoGeo == NULL) {
        fprintf(stderr, "Erro: nao foi possivel montar o caminho do .geo.\n");
        return 1;
    }

    dadosGeo = criar_dados_arquivo(caminhoGeo);
    if (dadosGeo == NULL) {
        fprintf(stderr, "Erro: nao foi possivel abrir o arquivo %s.\n",
                caminhoGeo);
        free(caminhoGeo);
        return 1;
    }

    quadras = tratar_arquivo_geo(dadosGeo, diretorioSaida);
    if (quadras == NULL) {
        fprintf(stderr, "Erro: falha ao processar o arquivo .geo ou gerar o SVG.\n");
        destruir_dados_arquivo(dadosGeo);
        free(caminhoGeo);
        return 1;
    }

    if (nomeArquivoVia != NULL) {
        caminhoVia = montar_caminho_entrada(diretorioEntrada, nomeArquivoVia);
        if (caminhoVia == NULL) {
            fprintf(stderr, "Erro: nao foi possivel montar o caminho do .via.\n");
            desaloca_geo(quadras);
            destruir_dados_arquivo(dadosGeo);
            free(caminhoGeo);
            return 1;
        }

        dadosArquivoVia = criar_dados_arquivo(caminhoVia);
        if (dadosArquivoVia == NULL) {
            fprintf(stderr, "Erro: nao foi possivel abrir o arquivo %s.\n",
                    caminhoVia);
            free(caminhoVia);
            desaloca_geo(quadras);
            destruir_dados_arquivo(dadosGeo);
            free(caminhoGeo);
            return 1;
        }

        dadosVia = tratar_arquivo_via(dadosArquivoVia);
        if (dadosVia == NULL) {
            fprintf(stderr, "Erro: falha ao processar o arquivo .via.\n");
            destruir_dados_arquivo(dadosArquivoVia);
            free(caminhoVia);
            desaloca_geo(quadras);
            destruir_dados_arquivo(dadosGeo);
            free(caminhoGeo);
            return 1;
        }
    }

    desaloca_via(dadosVia);
    destruir_dados_arquivo(dadosArquivoVia);
    free(caminhoVia);

    desaloca_geo(quadras);
    destruir_dados_arquivo(dadosGeo);
    free(caminhoGeo);

    return 0;
}
