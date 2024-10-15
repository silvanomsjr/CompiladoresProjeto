// #include "analise_lexica.h"
#include "analise_sintatica.h"

int main() {
  const char *entrada =
      "main programaTeste begin int -> x , y ; x := 4.5 ; if ( x == 10 ) "
      "then  y := x + 15 else { comentario } end";
  inicia(entrada);
  return 0;
}
