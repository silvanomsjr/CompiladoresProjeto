#include "analise_sintatica.h"
#include "analise_lexica.h"
#include "arvore.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Variaveis Globais
char *tipos_token[] = {
    "Palavra Reservada", "Identificador", "Atribuicao", "Relacional", "Paren",
    "Aritmetico",        "Pontuacao",     "Associacao", "Espaco",     "Erro",
    "INT_CONST",         "CHAR_CONST",    "FLOAT_CONST"};
TabelaSimbolos *tabela;
ArvoreBin *raiz;
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
  if (lookahead_count < tabela->tamanho) {
    lookahead_count++;
    return tabela->tokens[lookahead_count];
  } else {
    fprintf(stderr, "Erro: Fim Inesperado");
    exit(EXIT_FAILURE);
  }
}

void checa_pilha() {
  if (strcmp(lookahead.lexema, "") != 0) {
    fprintf(stderr, "Erro: Código continua após o 'end' do bloco main\n");
    exit(EXIT_FAILURE);
  }
}

Node *insere_no(Node *pai, char *val) {
  if (pai == NULL) {
    fprintf(stderr,
            "Erro: Nao foi possível alocar memoria suficiente para o pai\n");

    exit(EXIT_FAILURE);
  }
  Node *filho = (Node *)malloc(sizeof(Node));
  filho->qntd_filhos = 0;
  strcpy(filho->token, val);

  int qntd_filhos = pai->qntd_filhos;

  pai->filhos[qntd_filhos] = filho;
  pai->qntd_filhos++;
  return filho;
}

void match(char *palavra, Node *pai) {
  if (strcmp(palavra, tipos_token[lookahead.tipo_token]) == 0 ||
      strcmp(palavra, trim(lookahead.lexema)) == 0) {
    inserir_em_no(pai, palavra);
    lookahead = avanca_lookahead();
  } else {
    fprintf(
        stderr,
        "Erro Sintático: match falhou, esperado '%s' mas encontrado '%s' -> "
        "linha: '%d', coluna: '%d'\n",
        palavra, trim(lookahead.lexema), lookahead.linha, lookahead.coluna);
    exit(EXIT_FAILURE);
  }
}

void programa() {
  strcpy((*raiz)->token, "main");
  if (strcmp(tipos_token[lookahead.tipo_token], "Identificador") == 0) {
    match("Identificador", *raiz);
    bloco(*raiz);
  }
}

void bloco(Node *pai) {
  if (strcmp(tipos_token[lookahead.tipo_token], "Palavra Reservada") == 0 &&
      strcmp(trim(lookahead.lexema), "begin") == 0) {

    Node *atual = insere_no(pai, "bloco");
    match("begin", atual);
    declaracoes(atual);
    comandos(atual);
    match("end", atual);
  } else {
    fprintf(stderr,
            "Erro Sintático: 'begin' esperado -> linha: '%d', coluna: '%d'\n",
            lookahead.linha, lookahead.coluna);
    exit(EXIT_FAILURE);
  }
}

void declaracoes(Node *pai) {
  if (strcmp(tipos_token[lookahead.tipo_token], "Palavra Reservada") == 0) {
    Node *atual = insere_no(pai, "declaracoes");
    declaracao(atual);
  }
}

void declaracao(Node *pai) {
  if (strcmp(tipos_token[lookahead.tipo_token], "Palavra Reservada") == 0) {
    Node *atual = insere_no(pai, "declaracao");
    tipo(atual);
    match(":", atual);
    lista_ids_1(atual);
    match(";", atual);
    declaracao(pai);
  }
}

void tipo(Node *pai) {
  if (strcmp(trim(lookahead.lexema), "int") == 0 ||
      strcmp(trim(lookahead.lexema), "float") == 0 ||
      strcmp(trim(lookahead.lexema), "char") == 0) {
    Node *atual = insere_no(pai, "tipo");
    match(trim(lookahead.lexema), atual);
  } else {
    fprintf(stderr,
            "Erro Sintático: Tipo esperado -> linha: '%d', coluna: '%d'\n",
            lookahead.linha, lookahead.coluna);

    exit(EXIT_FAILURE);
  }
}

void lista_ids_1(Node *pai) {
  if (strcmp(tipos_token[lookahead.tipo_token], "Identificador") == 0) {
    Node *atual = insere_no(pai, "lista_ids_1");
    match("Identificador", atual);
    lista_ids_2(atual);
  } else {
    fprintf(
        stderr,
        "Erro Sintático: Identificador esperado -> linha: '%d', coluna: '%d'\n",
        lookahead.linha, lookahead.coluna);
    exit(EXIT_FAILURE);
  }
}

void lista_ids_2(Node *pai) {
  if (strcmp(trim(lookahead.lexema), ",") == 0) {
    Node *atual = insere_no(pai, "lista_ids_2");
    match(",", atual);
    lista_ids_1(atual);
  }
}

void comandos(Node *pai) {
  while (strcmp(tipos_token[lookahead.tipo_token], "Palavra Reservada") == 0 &&
             (strcmp(trim(lookahead.lexema), "if") == 0 ||
              strcmp(trim(lookahead.lexema), "while") == 0 ||
              strcmp(trim(lookahead.lexema), "repeat") == 0) ||
         strcmp(tipos_token[lookahead.tipo_token], "Identificador") == 0) {
    Node *atual = insere_no(pai, "comandos");
    comando(atual);
  }
}

void comando(Node *pai) {
  if (strcmp(tipos_token[lookahead.tipo_token], "Identificador") == 0) {
    Node *atual = insere_no(pai, "comando");
    atribuicao(atual);
  } else if (strcmp(trim(lookahead.lexema), "if") == 0) {
    Node *atual = insere_no(pai, "comando");
    comando_if(atual);
  } else if (strcmp(trim(lookahead.lexema), "while") == 0) {
    Node *atual = insere_no(pai, "comando");
    comando_repeticao(atual);
  } else if (strcmp(trim(lookahead.lexema), "begin") == 0) {
    Node *atual = insere_no(pai, "comando");
    bloco(atual);
  } else {
    fprintf(stderr,
            "Erro Sintático: comando esperado -> linha: '%d', coluna: '%d'\n",
            lookahead.linha, lookahead.coluna);
    exit(EXIT_FAILURE);
  }
}

void atribuicao(Node *pai) {
  if (strcmp(tipos_token[lookahead.tipo_token], "Identificador") == 0) {
    Node *atual = insere_no(pai, "atribuicao");
    match("Identificador", atual);
    match(":=", atual);
    expressao_1(atual);
    match(";", atual);
  }
}

void comando_if(Node *pai) {
  if (strcmp(tipos_token[lookahead.tipo_token], "Palavra Reservada") == 0 &&
      strcmp(trim(lookahead.lexema), "if") == 0) {
    match("if", pai);
    match("(", pai);
    condicao(pai);
    match(")", pai);
    match("then", pai);
    comando(pai);
    comando_else(pai);
  }
}

void comando_else(Node *pai) {
  if (strcmp(tipos_token[lookahead.tipo_token], "Palavra Reservada") == 0 &&
      strcmp(trim(lookahead.lexema), "else") == 0) {
    match("else", pai);
    comando(pai);
  }
}

void comando_repeticao(Node *pai) {
  if (strcmp(trim(lookahead.lexema), "while") == 0) {
    Node *atual = insere_no(pai, "while");
    match("while", atual);
    match("(", atual);
    condicao(atual);
    match(")", atual);
    match("do", atual);
    comando(atual);
  } else if (strcmp(trim(lookahead.lexema), "repeat") == 0) {
    Node *atual = insere_no(pai, "repeat");
    match("repeat", atual);
    comando(atual);
    match("until", atual);
    match("(", atual);
    condicao(atual);
    match(")", atual);
    match(";", atual);
  }
}

void condicao(Node *pai) {
  Node *atual = insere_no(pai, "condicao");
  expressao_1(atual);
  operador_relacional(atual);
  expressao_1(atual);
}

void operador_relacional(Node *pai) {
  if (strcmp(trim(lookahead.lexema), "==") == 0 ||
      strcmp(trim(lookahead.lexema), "!=") == 0 ||
      strcmp(trim(lookahead.lexema), ">") == 0 ||
      strcmp(trim(lookahead.lexema), ">=") == 0 ||
      strcmp(trim(lookahead.lexema), "<") == 0 ||
      strcmp(trim(lookahead.lexema), "<=") == 0) {
    Node *atual = insere_no(pai, "relacional");
    match(trim(lookahead.lexema), atual);
  } else {
    fprintf(stderr,
            "Erro Sintático: Operador relacional esperado -> linha: '%d', "
            "coluna: '%d'\n",
            lookahead.linha, lookahead.coluna);
    exit(EXIT_FAILURE);
  }
}

void expressao_1(Node *pai) {
  Node *atual = insere_no(pai, "expressao_1");
  termo_1(atual);
  expressao_2(atual);
}

void expressao_2(Node *pai) {
  if (strcmp(trim(lookahead.lexema), "+") == 0) {
    Node *atual = insere_no(pai, "expressao_2");
    match("+", atual);
    termo_1(atual);
    expressao_2(atual);
  } else if (strcmp(trim(lookahead.lexema), "-") == 0) {
    Node *atual = insere_no(pai, "expressao_2");
    match("-", atual);
    termo_1(atual);
    expressao_2(atual);
  }
}

void termo_1(Node *pai) {
  Node *atual = insere_no(pai, "termo_1");
  fator(atual);
  termo_2(atual);
}

void termo_2(Node *pai) {
  if (strcmp(trim(lookahead.lexema), "*") == 0) {
    Node *atual = insere_no(pai, "termo_2");
    match("*", atual);
    fator(atual);
    termo_2(atual);
  } else if (strcmp(trim(lookahead.lexema), "/") == 0) {
    Node *atual = insere_no(pai, "termo_2");
    match("/", atual);
    fator(atual);
    termo_2(atual);
  } else if (strcmp(trim(lookahead.lexema), "**") == 0) {
    Node *atual = insere_no(pai, "termo_2");
    match("**", atual);
    fator(atual);
    termo_2(atual);
  }
}

void fator(Node *pai) {
  if (strcmp(trim(lookahead.lexema), "(") == 0) {
    Node *atual = insere_no(pai, "fator");
    match("(", atual);
    expressao_1(atual);
    match(")", atual);
  } else if (strcmp(tipos_token[lookahead.tipo_token], "Identificador") == 0) {
    Node *atual = insere_no(pai, "fator");
    match("Identificador", atual);
  } else if (strcmp(tipos_token[lookahead.tipo_token], "FLOAT_CONST") == 0) {
    Node *atual = insere_no(pai, "fator");
    match("FLOAT_CONST", atual);
  } else if (strcmp(tipos_token[lookahead.tipo_token], "INT_CONST") == 0) {
    Node *atual = insere_no(pai, "fator");
    match("INT_CONST", atual);
  } else if (strcmp(tipos_token[lookahead.tipo_token], "CHAR_CONST") == 0) {
    Node *atual = insere_no(pai, "fator");
    match("CHAR_CONST", atual);
  } else {
    fprintf(stderr,
            "Erro Sintático: Fator esperado -> linha: '%d', coluna: '%d'\n",
            lookahead.linha, lookahead.coluna);
    exit(EXIT_FAILURE);
  }
}

void inicia(const char *entrada) {
  tabela = inicializar_tabela();

  processar(entrada, tabela);

  raiz = cria_arvore();

  // for (int i = 0; i < tabela->tamanho; i++) {
  //   imprimir_token(tabela->tokens[i]);
  // }

  Token atual = tabela->tokens[0];
  lookahead = tabela->tokens[1];

  if (strcmp(tipos_token[atual.tipo_token], "Palavra Reservada") == 0 &&
      strcmp(atual.lexema, "main") == 0) {
    programa();
  }
  checa_pilha();
  imprimir_arvore(*raiz, "");
  libera_arvore(raiz);
  free(tabela->tokens);
}
