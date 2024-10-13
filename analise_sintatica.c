#include "analise_sintatica.h"
#include "analise_lexica.h"
#include "arvore.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Variaveis Globais
char *tipos_token[] = {"Palavra Reservada", "Identificador", "Atribuicao",
                       "Relacional",        "Aritmetico",    "Pontuacao",
                       "Associacao",        "Espaco",        "Erro",
                       "INT_CONST",         "CHAR_CONST",    "FLOAT_CONST"};
TabelaSimbolos tabela;
ArvoreBin *raiz;
Node *atual;
Node *antigo;
Token lookahead;
int lookahead_count = 1;

char *trim(char *str) {
  // Remove espaços em branco do início
  while (isspace((unsigned char)*str))
    str++;

  // Se a string estiver vazia, retornar
  if (*str == 0)
    return str;

  // Remove espaços em branco do final
  char *end = str + strlen(str) - 1;
  while (end > str && isspace((unsigned char)*end))
    end--;

  // Adiciona o terminador nulo ao final da nova string
  *(end + 1) = '\0';

  return str;
}

Token avanca_lookahead() {
  if (lookahead_count < tabela.tamanho) {
    lookahead_count++;
    return tabela.tokens[lookahead_count];
  } else {
    fprintf(stderr, "Erro: Fim dos Tokens");
    exit(EXIT_FAILURE);
  }
}

void insere_no(char *val) {
  antigo = atual;
  inserir_em_no(antigo, val);
  int qntd_filhos = antigo->qntd_filhos;
  atual = antigo->filhos[qntd_filhos - 1];
}

void match(char *palavra) {
  if (strcmp(palavra, tipos_token[lookahead.tipo_token]) ||
      strcmp(palavra, trim(lookahead.lexema))) {
    lookahead = avanca_lookahead();
  } else {
    fprintf(stderr,
            "Erro Sintático: match falhou, esperado '%s' mas encontrado '%s'\n",
            palavra, trim(lookahead.lexema));
  }
}

void programa() {
  strcpy((*raiz)->token, "main");

  printf("programa\n");
  if (strcmp(tipos_token[lookahead.tipo_token], "Identificador") == 0) {
    match("Identificador");
    bloco();
  }
}

void bloco() {
  printf("bloco\n");
  insere_no("bloco");
  if (strcmp(tipos_token[lookahead.tipo_token], "Palavra Reservada") == 0 &&
      strcmp(trim(lookahead.lexema), "begin") == 0) {
    match("begin");
    declaracoes();
    comandos();
    match("end");
  } else {
    fprintf(stderr, "Erro Sintático: 'begin' esperado\n");
  }
}

void declaracoes() {
  printf("delcaracoes\n");
  insere_no("declaracoes");
  if (strcmp(tipos_token[lookahead.tipo_token], "Palavra Reservada") == 0) {
    tipo();
    match("->");
    lista_ids_1();
    match(";");
  }
}

void tipo() {
  printf("tipo\n");
  insere_no("tipo");
  if (strcmp(trim(lookahead.lexema), "int") == 0 ||
      strcmp(trim(lookahead.lexema), "float") == 0 ||
      strcmp(trim(lookahead.lexema), "char") == 0) {
    match(trim(lookahead.lexema));
  } else {
    fprintf(stderr, "Erro Sintático: Tipo esperado\n");
  }
}

void lista_ids_1() {
  printf("lista_ids_1\n");
  insere_no("lista_ids_1");
  if (strcmp(tipos_token[lookahead.tipo_token], "Identificador") == 0) {
    match("Identificador");
    lista_ids_2();
  } else {
    fprintf(stderr, "Erro Sintático: Identificador esperado\n");
  }
}

void lista_ids_2() {
  printf("lista_ids_2\n");
  insere_no("lista_ids_2");
  if (strcmp(trim(lookahead.lexema), ",") == 0) {
    match(",");
    lista_ids_1();
  }
  // Caso contrário é vazio (epsilon)
}

void comandos() {
  printf("comandos\n");
  insere_no("comandos");
  while (strcmp(tipos_token[lookahead.tipo_token], "Palavra Reservada") == 0 &&
             (strcmp(trim(lookahead.lexema), "if") == 0 ||
              strcmp(trim(lookahead.lexema), "while") == 0 ||
              strcmp(trim(lookahead.lexema), "repeat") == 0) ||
         strcmp(tipos_token[lookahead.tipo_token], "Identificador") == 0) {
    comando(); // Executa um comando
  }
}

void comando() {
  printf("comando\n");
  insere_no("comando");
  // printf("O que eh aqui? [%s]\n", tipos_token[lookahead.tipo_token]);
  // printf("lexema? [%s]\n", trim(lookahead.lexema));
  if (strcmp(tipos_token[lookahead.tipo_token], "Identificador") == 0) {
    atribuicao();
  } else if (strcmp(trim(lookahead.lexema), "if") == 0) {
    comando_if();
  } else if (strcmp(trim(lookahead.lexema), "while") == 0) {
    comando_repeticao();
  } else {
    fprintf(stderr, "Erro Sintático: comando esperado\n");
  }
}

void atribuicao() {
  printf("atr\n");
  insere_no("atribuicao");
  if (strcmp(tipos_token[lookahead.tipo_token], "Identificador") == 0) {
    match("Identificador");
    match(":=");
    expressao_1();
    match(";");
  }
}

void comando_if() {
  printf("if\n");
  insere_no("comando_if");
  if (strcmp(tipos_token[lookahead.tipo_token], "Palavra Reservada") == 0 &&
      strcmp(trim(lookahead.lexema), "if") == 0) {
    match("if");
    match("(");
    condicao();
    match(")");
    match("then");
    comando();
    comando_else();
  }
}

void comando_else() {
  printf("else\n");
  insere_no("comando_else");
  if (strcmp(tipos_token[lookahead.tipo_token], "Palavra Reservada") == 0 &&
      strcmp(trim(lookahead.lexema), "else")) {
    match("else");
    comando();
  }
}

void comando_repeticao() {
  printf("repet\n");
  insere_no("comando_repeticao");
  if (strcmp(trim(lookahead.lexema), "while") == 0) {
    match("while");
    match("(");
    condicao();
    match(")");
    match("do");
    comando();
  } else if (strcmp(trim(lookahead.lexema), "repeat") == 0) {
    match("repeat");
    comando();
    match("until");
    match("(");
    condicao();
    match(")");
    match(";");
  }
}

void condicao() {
  printf("condicao\n");
  insere_no("condicao");
  expressao_1();
  operador_relacional();
  expressao_1();
}

void operador_relacional() {
  printf("operador_relacional\n");
  insere_no("operador_relacional");
  if (strcmp(trim(lookahead.lexema), "==") == 0 ||
      strcmp(trim(lookahead.lexema), "!=") == 0 ||
      strcmp(trim(lookahead.lexema), ">") == 0 ||
      strcmp(trim(lookahead.lexema), ">=") == 0 ||
      strcmp(trim(lookahead.lexema), "<") == 0 ||
      strcmp(trim(lookahead.lexema), "<=") == 0) {
    match(trim(lookahead.lexema));
  } else {
    fprintf(stderr, "Erro Sintático: Operador relacional esperado\n");
  }
}

void expressao_1() {
  printf("expressao_1\n");
  insere_no("expressao_1");
  termo_1();
  expressao_2();
}

void expressao_2() {
  printf("expressao_2\n");
  insere_no("expressao_2");
  if (strcmp(trim(lookahead.lexema), "+") == 0) {
    match("+");
    termo_1();
    expressao_2();
  } else if (strcmp(trim(lookahead.lexema), "-") == 0) {
    match("-");
    termo_1();
    expressao_2();
  }
}

void termo_1() {
  printf("termo_1\n");
  insere_no("termo_1");
  fator();
  termo_2();
}

void termo_2() {
  printf("termo_2\n");
  insere_no("termo_2");
  if (strcmp(trim(lookahead.lexema), "*") == 0) {
    match("*");
    fator();
    termo_2();
  } else if (strcmp(trim(lookahead.lexema), "/") == 0) {
    match("/");
    fator();
    termo_2();
  } else if (strcmp(trim(lookahead.lexema), "**") == 0) {
    match("**");
    fator();
    termo_2();
  }
}

void fator() {
  printf("fator\n");
  insere_no("fator");
  printf("-- Lexema: [%s]\n", lookahead.lexema);
  printf("-- TipoToken: [%s]\n", tipos_token[lookahead.tipo_token]);
  if (strcmp(trim(lookahead.lexema), "(") == 0) {
    match("(");
    expressao_1();
    match(")");
  } else if (strcmp(tipos_token[lookahead.tipo_token], "Identificador") == 0) {
    match("Identificador");
  } else if (strcmp(tipos_token[lookahead.tipo_token], "FLOAT_CONST") == 0) {
    match("FLOAT_CONST");
  } else if (strcmp(tipos_token[lookahead.tipo_token], "INT_CONST") == 0) {
    match("INT_CONST");
  } else if (strcmp(tipos_token[lookahead.tipo_token], "CHAR_CONST") == 0) {
    match("CHAR_CONST");
  } else {
    fprintf(stderr, "Erro Sintático: Fator esperado\n");
  }
}

void inicia(const char *entrada) {
  inicializar_tabela(&tabela);

  processar(entrada, &tabela);

  raiz = cria_arvore();
  atual = *raiz;

  printf("Tokens reconhecidos:\n");
  for (int i = 0; i < tabela.tamanho; i++) {
    imprimir_token(tabela.tokens[i]);
  }

  Token atual = tabela.tokens[0];
  lookahead = tabela.tokens[1];

  if (strcmp(tipos_token[atual.tipo_token], "Palavra Reservada") == 0 &&
      strcmp(atual.lexema, "main") == 0) {
    programa();
  }
  // imprimir_arvore(*raiz, "");
  free(tabela.tokens);
}
