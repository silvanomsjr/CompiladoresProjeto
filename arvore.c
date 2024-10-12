#include "arvore.h"
#include <stdlib.h>

ArvoreBin *cria_arvore() {
  ArvoreBin *raiz = (ArvoreBin *)malloc(sizeof(ArvoreBin));
  if (raiz != NULL)
    *raiz = NULL;
  else
    return NULL;
  return raiz;
}

void libera_no(Node *no) {
  if (no == NULL)
    return;

  for (int i = 0; i < MAX_NODES; i++) {
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

void inserir_em_no(Node *no) {
  int quantidade_atual = no->qntd_filhos;
  no->qntd_filhos = quantidade_atual + 1;
  // no->filhos[no->qntd_filhos];
}
