#include "arvore.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ArvoreBin *cria_arvore() {
  // Aloca memória para o ponteiro da árvore
  ArvoreBin *raiz = (ArvoreBin *)malloc(sizeof(ArvoreBin));
  if (raiz == NULL) {
    fprintf(stderr, "Erro ao alocar memória para a árvore.\n");
    exit(1);
  }

  // Aloca memória para o nó raiz
  *raiz = (Node *)malloc(sizeof(Node));
  if (*raiz == NULL) {
    fprintf(stderr, "Erro ao alocar memória para o nó raiz.\n");
    free(raiz); // Libera o ponteiro da árvore se a alocação do nó falhar
    exit(1);
  }

  // Inicializa os campos da raiz
  (*raiz)->qntd_filhos = 0;
  strcpy((*raiz)->token, ""); // Inicializa o token com string vazia

  return raiz; // Retorna o ponteiro para a árvore
}

void libera_no(Node *no) {
  if (no == NULL) {
    return;
  }

  for (int i = 0; i < no->qntd_filhos; i++) {
    libera_no(no->filhos[i]);
  }
  free(no);
}

void libera_arvore(ArvoreBin *raiz) {
  if (raiz == NULL) {
    return;
  }
  libera_no(*raiz);
  free(raiz);
}

void inserir_em_no(Node *no, char *valor) {
  Node *novo_no = (Node *)malloc(sizeof(Node));
  if (novo_no == NULL) {
    fprintf(stderr, "Erro ao alocar novo no\n");
    exit(1);
  }

  novo_no->qntd_filhos = 0;
  strcpy(novo_no->token, valor);

  no->filhos[no->qntd_filhos] = novo_no;
  no->qntd_filhos++;
}

void imprimir_arvore(ArvoreBin raiz, char *indentacao) {
  if (raiz == NULL) {
    return;
  }
  // Imprime o nó atual com a indentação atual
  printf("%s|-- %s\n", indentacao, raiz->token);

  // Gera a nova indentação para os filhos (adiciona "|   ")
  char nova_indentacao[MAX_CHAR];
  strcpy(nova_indentacao, indentacao); // Copia a indentação atual
  strcat(nova_indentacao, "|   ");     // Adiciona a nova indentação

  // Chama recursivamente para os filhos, passando a nova indentação
  for (int i = 0; i < raiz->qntd_filhos; i++) {
    imprimir_arvore(raiz->filhos[i], nova_indentacao);
  }
}
