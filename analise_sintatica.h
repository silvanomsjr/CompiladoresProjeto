#ifndef ANALISE_SINTATICA_H
#define ANALISE_SINTATICA_H
#include "analise_lexica.h"
#include "arvore.h"

void inicia(const char *entrada);
void match(char *palavra, Node *no);
Token avanca_lookahead();
void programa();
void bloco(Node *no_pai);
void declaracoes(Node *no_pai);
void tipo(Node *no_pai);
void lista_ids_1(Node *no_pai);
void lista_ids_2(Node *no_pai);
void comandos(Node *no_pai);
void lista_comandos(Node *no_pai);
void comando(Node *no_pai);
void atribuicao(Node *no_pai);
void comando_if(Node *no_pai);
void comando_else(Node *no_pai);
void comando_repeticao(Node *no_pai);
void condicao(Node *no_pai);
void operador_relacional(Node *no_pai);
void expressao_1(Node *no_pai);
void expressao_2(Node *no_pai);
void termo_1(Node *no_pai);
void termo_2(Node *no_pai);
void fator(Node *no_pai);

#endif // !ANALISE_SINTATICA_H
