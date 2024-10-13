#ifndef ANALISE_SINTATICA_H
#define ANALISE_SINTATICA_H
#include "analise_lexica.h"

void inicia(const char *entrada);
void match(char *palavra);
Token avanca_lookahead();
void programa();
void bloco();
void declaracoes();
void tipo();
void lista_ids_1();
void lista_ids_2();
void comandos();
void comando();
void atribuicao();
void comando_if();
void comando_else();
void comando_repeticao();
void condicao();
void operador_relacional();
void expressao_1();
void expressao_2();
void termo_1();
void termo_2();
void fator();

#endif // !ANALISE_SINTATICA_H
