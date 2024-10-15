#ifndef ANALISE_SINTATICA_H
#define ANALISE_SINTATICA_H
#include "analise_lexica.h"
#include "arvore.h"

void inicia(const char *entrada);
void match(char *palavra, Node *pai);
void checa_pilha();
Token avanca_lookahead();
void programa();
void bloco(Node *pai);
void declaracoes(Node *pai);
void declaracao(Node *pai);
void tipo(Node *pai);
void lista_ids_1(Node *pai);
void lista_ids_2(Node *pai);
void comandos(Node *pai);
void comando(Node *pai);
void atribuicao(Node *pai);
void comando_if(Node *pai);
void comando_else(Node *pai);
void comando_repeticao(Node *pai);
void condicao(Node *pai);
void operador_relacional(Node *pai);
void expressao_1(Node *pai);
void expressao_2(Node *pai);
void termo_1(Node *pai);
void termo_2(Node *pai);
void fator(Node *pai);

#endif // !ANALISE_SINTATICA_H
