#include "analise_sintatica.h"
#include "arvore.h"

char parser_lookahead;

int match(char simbolo) {
  if (simbolo == parser_lookahead) {
    //insere nó
    return 1;
  }
  return 0;
}


