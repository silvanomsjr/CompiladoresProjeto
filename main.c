// #include "analise_lexica.h"
#include "analise_sintatica.h"
#include <stdio.h>
#include <stdlib.h>

char *ler_arquivo(const char *nome_arquivo) {
  FILE *arquivo = fopen(nome_arquivo, "rb");
  if (arquivo == NULL) {
    fprintf(stderr, "Erro ao abrir o arquivo %s\n", nome_arquivo);
    return NULL;
  }

  fseek(arquivo, 0, SEEK_END);
  long tamanho_arquivo = ftell(arquivo);
  rewind(arquivo);

  char *conteudo = (char *)malloc((tamanho_arquivo + 1) * sizeof(char));
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

  conteudo[tamanho_arquivo] = '\0';

  fclose(arquivo);

  return conteudo;
}

int main() {
  printf("primeiro\n");
  fflush(stdout);
  char *entrada = ler_arquivo("teste.txt");
  printf("segundo\n");
  fflush(stdout);
  inicia(entrada);

  return 0;
}
