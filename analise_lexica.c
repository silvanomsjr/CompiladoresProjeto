#include "analise_lexica.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *palavras_reservadas[] = {
    "main", "begin", "end",   "int", "char",   "float", "if",
    "then", "else",  "while", "do",  "repeat", "until",
};

// Adiciona um token à Tabela de Símbolos
void adicionar_token(TabelaSimbolos *tabela, Token token) {
  if (tabela == NULL || tabela->tokens == NULL) {
    fprintf(stderr, "Erro tabela de simbolos NULA!\n");
    exit(EXIT_FAILURE);
  }
  if (tabela->tamanho >= tabela->capacidade) {
    tabela->capacidade *= 2;
    Token *nova_memoria =
        realloc(tabela->tokens, tabela->capacidade * sizeof(Token));
    if (!nova_memoria) {
      fprintf(stderr, "Erro ao realocar memoria para a tabela de simbolos.\n");
      exit(1);
    }
    tabela->tokens = nova_memoria;
  }
  tabela->tokens[tabela->tamanho++] = token;
}

// Imprime um token no console
void imprimir_token(Token token) {
  const char *tipos[] = {
      "Palavra Reservada", "Identificador", "Atribuicao", "Relacional", "Paren",
      "Aritmetico",        "Pontuacao",     "Associacao", "Espaco",     "Erro",
      "INT_CONST",         "CHAR_CONST",    "FLOAT_CONST"};
  printf("Token: Tipo= %s, Lexema= [%s], Linha= %d, Coluna= %d",
         tipos[token.tipo_token], token.lexema, token.linha, token.coluna);
  if (token.tipo_dado == INT)
    printf(", Valor= %d", token.valor.valor_int);
  else if (token.tipo_dado == FLOAT)
    printf(", Valor= %f", token.valor.valor_float);
  else if (token.tipo_dado == CHAR)
    printf(", Valor= %c", token.valor.valor_char);
  printf("\n");
}

// Função de transição para a máquina de estados
int transicao(int estado, char simbolo) {
  switch (estado) {
  case 0:
    if (isalpha(simbolo) || simbolo == '_')
      return 1; // Identificador ou palavra reservada
    if (isdigit(simbolo))
      return 2; // Número inteiro
    if (simbolo == ':')
      return 3; // Atribuição
    if (simbolo == '\'')
      return 7; // Constante de caractere
    if (simbolo == '.')
      return 8; // Float
    if (strchr("=><!", simbolo))
      return 5; // Relacional
    if (strchr(";,", simbolo))
      return 13; // Pontuação
    if (strchr("()", simbolo))
      return 14; // Parenteses
    if (strchr("+-*/", simbolo))
      return 11; // Operador Matematico
    if (simbolo == '{')
      return 6; // Comentário
    if (isspace(simbolo))
      return 0; // Ignorar espaços
    return -1;  // Erro léxico
  case 1:
    if (isalnum(simbolo) || simbolo == '_')
      return 1; // Continua identificador ou palavra reservada
    return 0;   // Finaliza identificador ou palavra reservada
  case 2:
    if (isdigit(simbolo))
      return 2; // Continua número inteiro
    if (simbolo == '.')
      return 8; // Início de um float
    return 0;   // Finaliza número
  case 3:
    if (simbolo == '=')
      return 4; // Atribuição :=
    return 0;   // ':' sem '='
  case 4:
    return 0; // Finaliza atribuição
  case 5:
    if (simbolo == '=')
      return 0; // Relacional composto (>=, <=, !=)
    return 0;   // Finaliza operador simples
  case 6:
    if (simbolo == '}')
      return 12; // Finaliza comentário
    return 6;    // Continua comentário
  case 7:
    if (simbolo == '\'')
      return 0; // Finaliza constante de caractere
    return 7;   // Continua constante de caractere
  case 8:
    if (isdigit(simbolo))
      return 9; // Continua número float
    return -1;  // Erro se não for um dígito
  case 9:
    if (isdigit(simbolo))
      return 9; // Continua número float
    return 0;   // Finaliza número float
  case 11:
    if (simbolo == '*')
      return 11;
    return 0; // Operador Matematico
  case 12:
    return 0; // Finaliza Comentario
  case 13:
    return 0; // Pontuacao
  case 14:
    return 0; // Parenteses
  default:
    return -1; // Erro léxico
  }
}

// Função para verificar se o lexema é uma palavra reservada
int e_palavra_reservada(const char *lexema) {
  for (int i = 0; i < NUM_PALAVRAS_RESERVADAS; i++) {
    if (strcmp(lexema, palavras_reservadas[i]) == 0)
      return 1;
  }
  return 0;
}

char procura_char(char *search, char *string) {
  for (int i = 0; i < strlen(string); i++) {
    if (strchr(search, string[i])) {
      return string[i];
    }
  }
  return '0';
}

// Função para processar a entrada e reconhecer tokens
void processar(const char *entrada, TabelaSimbolos *tabela) {
  int estado = 0;
  int linha = 1, coluna = 1;
  char lexema[MAX_TOKEN_LENGTH] = {0};
  int lexema_length = 0;
  int comentario_aberto[2] = {0};

  const char *p = entrada;

  while (*p) {
    char simbolo = *p;
    int novo_estado = transicao(estado, simbolo);
    if (novo_estado == -1) {
      erro_lexico(simbolo, linha, coluna);
      estado = 0;
      lexema_length = 0;
      memset(lexema, 0, sizeof(lexema));
    } else {
      if (novo_estado == 0) {
        if (estado == 1) {
          Token token;
          strcpy(token.lexema, lexema);
          if (e_palavra_reservada(token.lexema)) {
            token.tipo_token = PALAVRA_RESERVADA;
          } else {
            token.tipo_token = IDENTIFICADOR;
          }

          token.linha = linha;
          token.coluna = coluna - lexema_length;
          token.tipo_dado = NAO_APLICAVEL;
          adicionar_token(tabela, token);
        } else if (estado == 2) {
          Token token;
          token.tipo_token = INT_CONST;
          strcpy(token.lexema, lexema);
          token.linha = linha;
          token.coluna = coluna - lexema_length;
          token.tipo_dado = INT;
          token.valor.valor_int = atoi(token.lexema);
          adicionar_token(tabela, token);
        } else if (estado == 9) {
          Token token;
          token.tipo_token = FLOAT_CONST;
          strncpy(token.lexema, lexema, lexema_length);
          token.lexema[lexema_length] = '\0';
          token.linha = linha;
          token.coluna = coluna - lexema_length;
          token.tipo_dado = FLOAT;
          token.valor.valor_float = atof(token.lexema);
          adicionar_token(tabela, token);
        } else if (estado == 7) {
          Token token;
          token.tipo_token = CHAR_CONST;
          token.lexema[0] = lexema[1];
          token.lexema[1] = '\0';
          token.linha = linha;
          token.coluna = coluna - lexema_length;
          token.tipo_dado = CHAR;
          token.valor.valor_char = lexema[0];
          adicionar_token(tabela, token);
        } else if (estado == 3) {
          Token token;
          token.tipo_token = ASSOCIACAO;
          strncpy(token.lexema, ":", 2);
          token.lexema[2] = '\0';
          token.linha = linha;
          token.coluna = coluna - 2;
          token.tipo_dado = NAO_APLICAVEL;
          adicionar_token(tabela, token);
        } else if (estado == 4) {
          Token token;
          token.tipo_token = ATRIBUICAO;
          strcpy(token.lexema, ":=");
          token.linha = linha;
          token.coluna = coluna - 2;
          token.tipo_dado = NAO_APLICAVEL;
          adicionar_token(tabela, token);
        } else if (estado == 5) {
          Token token;
          token.tipo_token = RELACIONAL;
          token.lexema[0] = lexema[0];
          if (simbolo == '=') {
            token.lexema[1] = '=';
            token.lexema[2] = '\0';
          } else {
            token.lexema[1] = '\0';
          }
          token.linha = linha;
          token.coluna = coluna - 2;
          token.tipo_dado = NAO_APLICAVEL;
          adicionar_token(tabela, token);
        } else if (estado == 11) {
          Token token;
          token.tipo_token = ARITMETICO;
          strcpy(token.lexema, lexema);
          token.linha = linha;
          token.coluna = coluna - 2;
          token.tipo_dado = NAO_APLICAVEL;
          adicionar_token(tabela, token);
        } else if (estado == 13) {
          Token token;
          token.lexema[0] = procura_char(",;", lexema);
          token.lexema[1] = '\0';
          token.tipo_token = PONTUACAO;
          token.tipo_dado = NAO_APLICAVEL;
          token.linha = linha;
          token.coluna = coluna - 2;
          adicionar_token(tabela, token);
        } else if (estado == 14) {
          Token token;
          token.lexema[0] = procura_char("()", lexema);
          token.lexema[1] = '\0';
          token.tipo_token = PAREN;
          token.tipo_dado = NAO_APLICAVEL;
          token.linha = linha;
          token.coluna = coluna - 2;
          adicionar_token(tabela, token);
        }
        lexema_length = 0;
        memset(lexema, 0, sizeof(lexema));
      }
      if (novo_estado == 6) {
        comentario_aberto[0] = linha;
        comentario_aberto[1] = coluna - 2;
      }
      if (novo_estado == 12) {
        comentario_aberto[0] = 0;
        comentario_aberto[1] = 0;
      }
      if (!isspace(simbolo) && simbolo != '{' && simbolo != '}')
        lexema[lexema_length++] = simbolo;
      estado = novo_estado;
    }
    if (simbolo == '\n') {
      linha++;
      coluna = 0;
    }
    coluna++;
    if (novo_estado == 0 && !isspace(simbolo) && simbolo != '{' &&
        simbolo != '}' && simbolo != '\'' && simbolo != '=') {
      lexema_length--;
    } else {
      p++;
    }
  }

  if (estado == 6 || comentario_aberto[0] != 0 || comentario_aberto[1] != 0) {
    fprintf(stderr,
            "Erro: comentario nao fechado na linha = %d, coluna = %d\n\n",
            comentario_aberto[0], comentario_aberto[1]);
    exit(EXIT_FAILURE);
  }
}

// Função para tratamento de erro léxico
void erro_lexico(char simbolo, int linha, int coluna) {
  fprintf(stderr,
          "Erro lexico: simbolo '%c' inesperado na linha %d, coluna %d\n",
          simbolo, linha, coluna);
  exit(EXIT_FAILURE);
}

// Inicializa a Tabela de Símbolos
TabelaSimbolos *inicializar_tabela() {
  TabelaSimbolos *tabela = (TabelaSimbolos *)malloc(sizeof(TabelaSimbolos));
  if (tabela == NULL) {
    perror("Erro ao alocar memória para a tabela de símbolos");
    exit(EXIT_FAILURE);
  }

  tabela->capacidade = 12;
  tabela->tamanho = 0;
  tabela->tokens = (Token *)malloc(12 * sizeof(Token));
  if (!tabela->tokens) {
    perror("Erro ao alocar memória para os tokens");
    free(tabela);
    exit(EXIT_FAILURE);
  }

  return tabela;
}
