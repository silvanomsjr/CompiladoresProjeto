#ifndef ANALISE_LEXICA_H
#define ANALISE_LEXICA_H

#include <stdio.h>

#define MAX_TOKEN_LENGTH 100
#define NUM_PALAVRAS_RESERVADAS 18

typedef enum {
  PALAVRA_RESERVADA,
  IDENTIFICADOR,
  NUMERO,
  ATRIBUICAO,
  RELACIONAL,
  ARITMETICO,
  PONTUACAO,
  ASSOCIACAO,
  COMENTARIO,
  ESPACO,
  ERRO,
  CHAR_CONST,
  FLOAT_CONST
} TipoToken;

typedef struct {
  TipoToken tipo_token;
  char lexema[MAX_TOKEN_LENGTH];
  int linha;
  int coluna;
  union {
    int valor_int;
    float valor_float;
    char valor_char;
  } valor;
  enum { INT, FLOAT, CHAR, NAO_APLICAVEL } tipo_dado;
} Token;

typedef struct {
  Token *tokens;
  int tamanho;
  int capacidade;
} TabelaSimbolos;

void adicionar_token(TabelaSimbolos *tabela, Token token);
void imprimir_token(Token token);
int transicao(int estado, char simbolo);
int processar(const char *entrada, TabelaSimbolos *tabela, int coment_aberto[]);
void erro_lexico(char simbolo, int linha, int coluna);
int e_palavra_reservada(const char *lexema);
void inicializar_tabela(TabelaSimbolos *tabela);

#endif // ANALISE_LEXICA_H
