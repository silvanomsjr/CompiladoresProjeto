#include "analise_lexica.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
  // Exemplos de entrada para teste
  const char *entrada =
      "main programaTeste  begin int -> x; x := 42; if x > 10 "
      "then  y := x + 1 else { comentario } end";
  // const char *entrada1 = "main programaTeste2  begin int->m m := 50 ; while m
  // > 10 do  {y := x + 1} repeat m until {comentario} end";

  TabelaSimbolos tabela;
  inicializar_tabela(&tabela);
  int coment_abert[2] = {0, 0};

  int estado_final = processar(entrada, &tabela, coment_abert); // teste 1
  //   processar(entrada1, &tabela); //teste 2

  printf("Tokens reconhecidos:\n");
  for (int i = 0; i < tabela.tamanho; i++) {
    imprimir_token(tabela.tokens[i]);
  }

  if (estado_final == 6) {
    printf("\n\nErro: comentário não fechado na linha = %d, coluna = %d\n\n",
           coment_abert[0], coment_abert[1]);
  }

  free(tabela.tokens);
  return 0;
}
