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

char teste[50][100];
int teste_c = 0;

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
  lookahead_count++;
  return tabela.tokens[lookahead_count];
}

void insere_no(char *val, Node *no_pai) {
  if (no_pai == NULL)
    return;
  inserir_em_no(no_pai, val);
}

void match(char *palavra, Node *no) {
  if (strcmp(palavra, tipos_token[lookahead.tipo_token]) ||
      strcmp(palavra, trim(lookahead.lexema))) {
    printf("Entrou\n-Palavra: [%s]\n-Lexema: [%s]\n\n", palavra,
           trim(lookahead.lexema));
    lookahead = avanca_lookahead();
  } else {
    fprintf(stderr,
            "Erro Sintático: match falhou, esperado '%s' mas encontrado '%s'\n",
            palavra, trim(lookahead.lexema));
  }
}

void programa() {
  strcpy((*raiz)->token, "main");

  if (strcmp(tipos_token[lookahead.tipo_token], "Identificador") == 0) {
    match("Identificador", (*raiz));
    bloco(*raiz);
  }
}

void bloco(Node *no_pai) {
  if (strcmp(tipos_token[lookahead.tipo_token], "Palavra Reservada") == 0 &&
      strcmp(trim(lookahead.lexema), "begin") == 0) {
    printf("que\n");
    Node *no_atual = (Node *)malloc(sizeof(Node));
    if (no_atual == NULL) {
      return;
    }
    int qntd_pai = no_pai->qntd_filhos;
    no_pai->filhos[qntd_pai] = no_atual;
    no_pai->qntd_filhos++;
    insere_no("bloco", no_atual);
    match("begin", no_atual);
    declaracoes(no_atual);
    comandos(no_atual);
    match("end", no_atual);
  } else {
    fprintf(stderr, "Erro Sintático: 'begin' esperado\n");
  }
  printf("tipos_token[lookahead.tipo_token]: [%s]\n",
         tipos_token[lookahead.tipo_token]);
  printf("lexema: [%s]\n\n", lookahead.lexema);
}

void declaracoes(Node *no_pai) {
  if (strcmp(tipos_token[lookahead.tipo_token], "Palavra Reservada") == 0) {
    Node *no_atual = (Node *)malloc(sizeof(Node));
    if (no_atual == NULL) {
      return;
    }
    int qntd_pai = no_pai->qntd_filhos;
    no_pai->filhos[qntd_pai] = no_atual;
    no_pai->qntd_filhos++;
    insere_no("declaracoes", no_atual);
    tipo(no_atual);
    match("->", no_atual);
    lista_ids_1(no_atual);
    match(";", no_atual);
  }
}

void tipo(Node *no_pai) {
  if (strcmp(trim(lookahead.lexema), "int") == 0 ||
      strcmp(trim(lookahead.lexema), "float") == 0 ||
      strcmp(trim(lookahead.lexema), "char") == 0) {
    Node *no_atual = (Node *)malloc(sizeof(Node));
    if (no_atual == NULL) {
      return;
    }
    int qntd_pai = no_pai->qntd_filhos;
    no_pai->filhos[qntd_pai] = no_atual;
    no_pai->qntd_filhos++;
    insere_no("tipo", no_atual);
    match(trim(lookahead.lexema), no_atual);
  } else {
    fprintf(stderr, "Erro Sintático: Tipo esperado\n");
  }
}

void lista_ids_1(Node *no_pai) {
  if (strcmp(tipos_token[lookahead.tipo_token], "Identificador") == 0) {
    Node *no_atual = (Node *)malloc(sizeof(Node));
    if (no_atual == NULL) {
      return;
    }
    int qntd_pai = no_pai->qntd_filhos;
    no_pai->filhos[qntd_pai] = no_atual;
    no_pai->qntd_filhos++;
    insere_no("lista_ids_1", no_atual);
    match("Identificador", no_atual);
    lista_ids_2(no_atual);
  } else {
    fprintf(stderr, "Erro Sintático: Identificador esperado\n");
  }
}

void lista_ids_2(Node *no_pai) {
  if (strcmp(trim(lookahead.lexema), ",") == 0) {
    Node *no_atual = (Node *)malloc(sizeof(Node));
    if (no_atual == NULL) {
      return;
    }
    int qntd_pai = no_pai->qntd_filhos;
    no_pai->filhos[qntd_pai] = no_atual;
    no_pai->qntd_filhos++;
    insere_no("lista_ids_2", no_atual);
    match(",", no_atual);
    lista_ids_1(no_atual);
  }
  // Caso contrário é vazio (epsilon)
}

void comandos(Node *no_pai) {
  if (strcmp(trim(lookahead.lexema), "if") == 0 ||
      strcmp(trim(lookahead.lexema), "while") == 0 ||
      strcmp(trim(lookahead.lexema), "repeat") == 0 ||
      strcmp(tipos_token[lookahead.tipo_token], "Identificador") == 0) {
    Node *no_atual = (Node *)malloc(sizeof(Node));
    if (no_atual == NULL) {
      return;
    }
    int qntd_pai = no_pai->qntd_filhos;
    no_pai->filhos[qntd_pai] = no_atual;
    no_pai->qntd_filhos++;
    insere_no("comandos", no_atual);
    comando(no_atual); // Executa um comando
  }
}

void comando(Node *no_pai) {
  if (strcmp(tipos_token[lookahead.tipo_token], "Identificador") == 0) {
    Node *no_atual = (Node *)malloc(sizeof(Node));
    if (no_atual == NULL) {
      return;
    }
    int qntd_pai = no_pai->qntd_filhos;
    no_pai->filhos[qntd_pai] = no_atual;
    no_pai->qntd_filhos++;
    insere_no("comando", no_atual);
    atribuicao(no_atual);
  } else if (strcmp(trim(lookahead.lexema), "if") == 0) {
    Node *no_atual = (Node *)malloc(sizeof(Node));
    if (no_atual == NULL) {
      return;
    }
    int qntd_pai = no_pai->qntd_filhos;
    no_pai->filhos[qntd_pai] = no_atual;
    no_pai->qntd_filhos++;
    insere_no("comando", no_atual);
    comando_if(no_atual);
  } else if (strcmp(trim(lookahead.lexema), "while") == 0) {
    Node *no_atual = (Node *)malloc(sizeof(Node));
    if (no_atual == NULL) {
      return;
    }
    int qntd_pai = no_pai->qntd_filhos;
    no_pai->filhos[qntd_pai] = no_atual;
    no_pai->qntd_filhos++;
    insere_no("comando", no_atual);
    comando_repeticao(no_atual);
    insere_no("comando", no_atual);
  } else {
    fprintf(stderr, "Erro Sintático: comando esperado\n");
  }
}

void atribuicao(Node *no_pai) {
  if (strcmp(tipos_token[lookahead.tipo_token], "Identificador") == 0) {
    Node *no_atual = (Node *)malloc(sizeof(Node));
    if (no_atual == NULL) {
      return;
    }
    int qntd_pai = no_pai->qntd_filhos;
    no_pai->filhos[qntd_pai] = no_atual;
    no_pai->qntd_filhos++;
    insere_no("atribuicao", no_atual);
    match("Identificador", no_atual);
    match(":=", no_atual);
    expressao_1(no_atual);
    match(";", no_atual);
  }
}

void comando_if(Node *no_pai) {
  if (strcmp(tipos_token[lookahead.tipo_token], "Palavra Reservada") == 0 &&
      strcmp(trim(lookahead.lexema), "if") == 0) {
    Node *no_atual = (Node *)malloc(sizeof(Node));
    if (no_atual == NULL) {
      return;
    }
    int qntd_pai = no_pai->qntd_filhos;
    no_pai->filhos[qntd_pai] = no_atual;
    no_pai->qntd_filhos++;
    insere_no("comando_if", no_atual);
    match("if", no_atual);
    match("(", no_atual);
    condicao(no_atual);
    match(")", no_atual);
    match("then", no_atual);
    comando(no_atual);
    comando_else(no_atual);
  }
}

void comando_else(Node *no_pai) {
  if (strcmp(tipos_token[lookahead.tipo_token], "Palavra Reservada") == 0 &&
      strcmp(trim(lookahead.lexema), "else")) {
    Node *no_atual = (Node *)malloc(sizeof(Node));
    if (no_atual == NULL) {
      return;
    }
    int qntd_pai = no_pai->qntd_filhos;
    no_pai->filhos[qntd_pai] = no_atual;
    no_pai->qntd_filhos++;
    insere_no("comando_else", no_atual);
    match("else", no_atual);
    comando(no_atual);
  }
}

void comando_repeticao(Node *no_pai) {
  Node *no_atual = (Node *)malloc(sizeof(Node));
  if (no_atual == NULL) {
    return;
  }
  int qntd_pai = no_pai->qntd_filhos;
  no_pai->filhos[qntd_pai] = no_atual;
  no_pai->qntd_filhos++;
  insere_no("comando_repeticao", no_atual);
  if (strcmp(trim(lookahead.lexema), "while") == 0) {
    match("while", no_atual);
    match("(", no_atual);
    condicao(no_atual);
    match(")", no_atual);
    match("do", no_atual);
    comando(no_atual);
  } else if (strcmp(trim(lookahead.lexema), "repeat") == 0) {
    match("repeat", no_atual);
    comando(no_atual);
    match("until", no_atual);
    match("(", no_atual);
    condicao(no_atual);
    match(")", no_atual);
    match(";", no_atual);
  }
}

void condicao(Node *no_pai) {
  Node *no_atual = (Node *)malloc(sizeof(Node));
  if (no_atual == NULL) {
    return;
  }
  int qntd_pai = no_pai->qntd_filhos;
  no_pai->filhos[qntd_pai] = no_atual;
  no_pai->qntd_filhos++;
  insere_no("condicao", no_atual);
  expressao_1(no_atual);
  operador_relacional(no_atual);
  expressao_1(no_atual);
}

void operador_relacional(Node *no_pai) {
  if (strcmp(trim(lookahead.lexema), "==") == 0 ||
      strcmp(trim(lookahead.lexema), "!=") == 0 ||
      strcmp(trim(lookahead.lexema), ">") == 0 ||
      strcmp(trim(lookahead.lexema), ">=") == 0 ||
      strcmp(trim(lookahead.lexema), "<") == 0 ||
      strcmp(trim(lookahead.lexema), "<=") == 0) {
    Node *no_atual = (Node *)malloc(sizeof(Node));
    if (no_atual == NULL) {
      return;
    }
    int qntd_pai = no_pai->qntd_filhos;
    no_pai->filhos[qntd_pai] = no_atual;
    no_pai->qntd_filhos++;
    insere_no("operador_relacional", no_atual);
    match(trim(lookahead.lexema), no_atual);
  } else {
    fprintf(stderr, "Erro Sintático: Operador relacional esperado\n");
  }
}

void expressao_1(Node *no_pai) {
  Node *no_atual = (Node *)malloc(sizeof(Node));
  if (no_atual == NULL) {
    return;
  }
  int qntd_pai = no_pai->qntd_filhos;
  no_pai->filhos[qntd_pai] = no_atual;
  no_pai->qntd_filhos++;
  insere_no("expressao_1", no_atual);
  termo_1(no_atual);
  expressao_2(no_atual);
}

void expressao_2(Node *no_pai) {
  Node *no_atual = (Node *)malloc(sizeof(Node));
  if (no_atual == NULL) {
    return;
  }
  int qntd_pai = no_pai->qntd_filhos;
  no_pai->filhos[qntd_pai] = no_atual;
  no_pai->qntd_filhos++;
  insere_no("expressao_2", no_atual);
  if (strcmp(trim(lookahead.lexema), "+") == 0) {
    match("+", no_atual);
    termo_1(no_atual);
    expressao_2(no_atual);
  } else if (strcmp(trim(lookahead.lexema), "-") == 0) {
    match("-", no_atual);
    termo_1(no_atual);
    expressao_2(no_atual);
  }
}

void termo_1(Node *no_pai) {
  Node *no_atual = (Node *)malloc(sizeof(Node));
  if (no_atual == NULL) {
    return;
  }
  int qntd_pai = no_pai->qntd_filhos;
  no_pai->filhos[qntd_pai] = no_atual;
  no_pai->qntd_filhos++;
  insere_no("termo_1", no_atual);
  fator(no_atual);
  termo_2(no_atual);
}

void termo_2(Node *no_pai) {
  Node *no_atual = (Node *)malloc(sizeof(Node));
  if (no_atual == NULL) {
    return;
  }
  int qntd_pai = no_pai->qntd_filhos;
  no_pai->filhos[qntd_pai] = no_atual;
  no_pai->qntd_filhos++;
  insere_no("termo_2", no_atual);
  if (strcmp(trim(lookahead.lexema), "*") == 0) {
    match("*", no_atual);
    fator(no_atual);
    termo_2(no_atual);
  } else if (strcmp(trim(lookahead.lexema), "/") == 0) {
    match("/", no_atual);
    fator(no_atual);
    termo_2(no_atual);
  } else if (strcmp(trim(lookahead.lexema), "**") == 0) {
    match("**", no_atual);
    fator(no_atual);
    termo_2(no_atual);
  }
}

void fator(Node *no_pai) {
  if (strcmp(trim(lookahead.lexema), "(") == 0) {
    Node *no_atual = (Node *)malloc(sizeof(Node));
    if (no_atual == NULL) {
      return;
    }
    int qntd_pai = no_pai->qntd_filhos;
    no_pai->filhos[qntd_pai] = no_atual;
    no_pai->qntd_filhos++;
    insere_no("fator", no_atual);
    match("(", no_atual);
    expressao_1(no_atual);
    match(")", no_atual);
  } else if (strcmp(tipos_token[lookahead.tipo_token], "Identificador") == 0) {
    Node *no_atual = (Node *)malloc(sizeof(Node));
    if (no_atual == NULL) {
      return;
    }
    int qntd_pai = no_pai->qntd_filhos;
    no_pai->filhos[qntd_pai] = no_atual;
    no_pai->qntd_filhos++;
    insere_no("fator", no_atual);
    match("Identificador", no_atual);
  } else if (strcmp(tipos_token[lookahead.tipo_token], "FLOAT_CONST") == 0) {
    Node *no_atual = (Node *)malloc(sizeof(Node));
    if (no_atual == NULL) {
      return;
    }
    int qntd_pai = no_pai->qntd_filhos;
    no_pai->filhos[qntd_pai] = no_atual;
    no_pai->qntd_filhos++;
    insere_no("fator", no_atual);
    match("FLOAT_CONST", no_atual);
  } else if (strcmp(tipos_token[lookahead.tipo_token], "INT_CONST") == 0) {
    Node *no_atual = (Node *)malloc(sizeof(Node));
    if (no_atual == NULL) {
      return;
    }
    int qntd_pai = no_pai->qntd_filhos;
    no_pai->filhos[qntd_pai] = no_atual;
    no_pai->qntd_filhos++;
    insere_no("fator", no_atual);
    match("INT_CONST", no_atual);
  } else if (strcmp(tipos_token[lookahead.tipo_token], "CHAR_CONST") == 0) {
    Node *no_atual = (Node *)malloc(sizeof(Node));
    if (no_atual == NULL) {
      return;
    }
    int qntd_pai = no_pai->qntd_filhos;
    no_pai->filhos[qntd_pai] = no_atual;
    no_pai->qntd_filhos++;
    insere_no("fator", no_atual);
    match("CHAR_CONST", no_atual);
  } else {
    fprintf(stderr, "Erro Sintático: Fator esperado\n");
  }
}

// void resolve_isso_agora() {
//   ArvoreBin *arv;
//   arv = cria_arvore();
//   strcpy((*arv)->token, "main");
//   inserir_em_no(*arv, "id");
//   inserir_em_no(*arv, "Bloco");
//   inserir_em_no((*arv)->filhos[1], "begin");
//   inserir_em_no((*arv)->filhos[1], "declaracoes");
//   inserir_em_no((*arv)->filhos[1], "comandos");
//   inserir_em_no((*arv)->filhos[1], "end");
//   inserir_em_no((*arv)->filhos[1]->filhos[1], ":");
//   inserir_em_no((*arv)->filhos[1]->filhos[1], "lista_ids_1");
//   inserir_em_no((*arv)->filhos[1]->filhos[1]->filhos[1], "id");
//   inserir_em_no((*arv)->filhos[1]->filhos[1]->filhos[1], "lista_ids_2");
//   inserir_em_no((*arv)->filhos[1]->filhos[1]->filhos[1]->filhos[1], ",");
//   inserir_em_no((*arv)->filhos[1]->filhos[1]->filhos[1]->filhos[1], "id");
//   inserir_em_no((*arv)->filhos[1]->filhos[1], ";");
//   inserir_em_no((*arv)->filhos[1]->filhos[2], "comando");
//   inserir_em_no((*arv)->filhos[1]->filhos[2]->filhos[0], "id");
//   inserir_em_no((*arv)->filhos[1]->filhos[2]->filhos[0], ":=");
//   inserir_em_no((*arv)->filhos[1]->filhos[2]->filhos[0], "expressao_1");
//   inserir_em_no((*arv)->filhos[1]->filhos[2], "comandos");
//   imprimir_arvore(*arv, "");
// }

void inicia(const char *entrada) {
  inicializar_tabela(&tabela);

  processar(entrada, &tabela);

  // printf("Tokens reconhecidos:\n");
  // for (int i = 0; i < tabela.tamanho; i++) {
  //   imprimir_token(tabela.tokens[i]);
  // }

  raiz = cria_arvore();
  atual = *raiz;

  Token atual = tabela.tokens[0];
  lookahead = tabela.tokens[1];

  if (strcmp(tipos_token[atual.tipo_token], "Palavra Reservada") == 0 &&
      strcmp(atual.lexema, "main") == 0) {
    programa();
  }
  inicia(entrada);

  free(tabela.tokens);
}
