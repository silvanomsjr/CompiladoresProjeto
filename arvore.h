#ifndef ARVORE_H
#define ARVORE_H

#define MAX_CHAR 100
#define MAX_NODES 20

typedef struct Node {
  char token[MAX_CHAR];
  int qntd_filhos;
  struct Node *filhos[MAX_NODES];
} Node;

typedef Node *ArvoreBin;

ArvoreBin *cria_arvore();
void libera_arvore(ArvoreBin *raiz);
void inserir_em_no(Node *no, char *valor);
void imprimir_arvore(ArvoreBin raiz, char *indentacao);

#endif // !ARVORE_H
