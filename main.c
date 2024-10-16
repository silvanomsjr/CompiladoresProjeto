// #include "analise_lexica.h"
#include "analise_sintatica.h"
#include <stdio.h>
#include <stdlib.h>

char *ler_arquivo(const char *nome_arquivo) {
  FILE *arquivo = fopen(nome_arquivo, "r");
  if (arquivo == NULL) {
    fprintf(stderr, "Erro ao abrir o arquivo %s\n", nome_arquivo);
    return NULL;
  }

  fseek(arquivo, 0, SEEK_END);
  long tamanho_arquivo = ftell(arquivo);
  rewind(arquivo);

  char *conteudo = (char *)malloc((tamanho_arquivo + 2) * sizeof(char));
  if (conteudo == NULL) {
    fprintf(stderr, "Erro ao alocar memória\n");
    fclose(arquivo);
    return NULL;
  }

  size_t lido = fread(conteudo, sizeof(char), tamanho_arquivo, arquivo);
  if (lido != tamanho_arquivo) {
    fprintf(stderr, "Erro ao ler o arquivo\n");
    free(conteudo);
    fclose(arquivo);
    return NULL;
  }
  if (lido == 0) {
    fprintf(stderr, "Erro, arquivo vazio\n");
    free(conteudo);
    fclose(arquivo);
    return NULL;
  }

  conteudo[tamanho_arquivo] = ' ';
  conteudo[tamanho_arquivo + 1] = '\0';

  fclose(arquivo);

  return conteudo;
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Uso: %s <nome_do_arquivo>\n", argv[0]);
    return EXIT_FAILURE;
  }
  char *entrada = ler_arquivo(argv[1]);
  if (entrada == NULL) {
    return EXIT_FAILURE;
  }
  inicia(entrada);

  return EXIT_SUCCESS;
}
