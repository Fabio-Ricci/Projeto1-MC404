#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "montador.h"
#include "token.h"

/*
Exemplo de erros:
const char* get_error_string (enum errors code) {
    switch (code) {
        case ERR_HEXADECIMAL_NAO_VALIDO:
            return "LEXICO: Numero Hexadecimal Inválido!";
        case ERRO_ROTULO_JA_DEFINIDO:
            return "GRAMATICAL: ROTULO JA FOI DEFINIDO!";
        case ERR_DIRETIVA_NAO_VALIDA:
            return "LEXICO: Diretiva não válida";
*/

/*
    ---- Você Deve implementar esta função para a parte 1.  ----
    Entrada da função: arquivo de texto lido e seu tamanho
    Retorna:
        * 1 caso haja erro na montagem;
        * 0 caso não haja erro.
*/

char *strtok1(char *s, char *delim) {
  if (strcmp(s,"") == 0){
    return NULL;
  }

  char *str;
  int i;

  for (i=0;strchr(delim, s[i]) == NULL;i++){}

  str = malloc((i+1)*sizeof(char));
  strncpy(str, s, i);
  str[i]='\0';
  if (i<strlen(s)) {
    strncpy(s, s + i + 1, strlen(s) + i);
    s[strlen(s)] = '\0';
  } else {
    s[0] = '\0';
  }
  return str;
}

/**
 * Cria um token e coloca no vetor de token
 */
void criaToken(char *palavra, TipoDoToken tipo, int numLinha) {
  Token token;
  token.tipo = tipo;
  token.linha = numLinha;
  token.palavra = malloc(sizeof(char));
  strcpy(token.palavra, palavra);
  adicionarToken(token);
}

/**
 * Métodos para validar palavra
 * retorna 1 se eh rotulo, diretiva, etc...
 * retorn 0 se nao eh
 */
int ehRotulo(char *palavra) {
  if (palavra[strlen(palavra) - 1] == ':') {
    if (palavra[0] == '.' || isdigit(palavra[0])) {
      return 0;  // rótulo inválido comecando com .
    }
    return 1;
  }
  return 0;
}

int ehDiretiva(char *palavra) {
  if (strcmp(palavra, ".org") == 0|| strcmp(palavra, ".align")  == 0||
      strcmp(palavra, ".wfill")  == 0|| strcmp(palavra, ".set")  == 0||
      strcmp(palavra, ".word") == 0) {
    return 1;
  }
  return 0;
}

int ehInstrucao(char *palavra) {
  char *intrucoes[19] = {"ld",     "ldinv", "ldabs",  "ldmq",  "ldmqmx",
                         "store",  "jump",  "jumpl",  "jumpr", "add",
                         "addabs", "sub",   "subabs", "mult",  "div",
                         "lsh",    "rsh",   "storal", "storar"};
  int i;
  for (i = 0; i < 19; i++) {
    if (strcmp(palavra, intrucoes[i]) == 0) {  // verifica se eh alguma das intrucoes
      return 1;
    }
  }
  return 0;
}

int ehHexadecimal(char *palavra) {
  if (strlen(palavra) <= 10) {
    if (palavra[0] == '0' && palavra[1] == 'x') {
      int i;
      for (i = 2; i < strlen(palavra); i++) {
        if (!isdigit(palavra[i])) {  // verifica se todos os digitos sao numeros
          return 0;
        }
      }
      return 1;
    }
  }
  return 0;
}

int ehDecimal(char *palavra) {
  if (strlen(palavra) <= 8) {
    int i;
    for (i = 0; i < strlen(palavra); i++) {
      if (!isdigit(palavra[i])) {  // verifica se todos os digitos sao numeros
        return 0;
      }
    }
    return 1;
  }
  return 0;
}

int ehNome(char *palavra) { return 0; }

char *trim(char *s)
{
  int i = 0;
  int j = strlen ( s ) - 1;
  int k = 0;

  while ( isspace ( s[i] ) && s[i] != '\0' )
    i++;

  while ( isspace ( s[j] ) && j >= 0 )
    j--;

  while ( i <= j )
    s[k++] = s[i++];

  s[k] = '\0';

  return s;
}

/**
 * Processa uma palavra
 * retorna numero da linha se palavra invalida
 * retorna 0 se palavra válida
 */
int processaPalavra(char *palavra, int numLinha) {
  trim(palavra);
  if (ehRotulo(palavra)) {
    criaToken(palavra, DefRotulo, numLinha);
    return 0;
  }
  if (ehDiretiva(palavra)) {
    criaToken(palavra, Diretiva, numLinha);
    return 0;
  }
  if (ehInstrucao(palavra)) {
    criaToken(palavra, Instrucao, numLinha);
    return 0;
  }
  if (ehHexadecimal(palavra)) {
    criaToken(palavra, Hexadecimal, numLinha);
    return 0;
  }
  if (ehDecimal(palavra)) {
    criaToken(palavra, Decimal, numLinha);
    return 0;
  }
  if (ehNome(palavra)) {
    criaToken(palavra, Nome, numLinha);
    return 0;
  }
  return numLinha;
}

char *eliminaComentarios(char *linha) {
  int i;
  for (i = 0; i<strlen(linha) && linha[i] != '#'; i++) {}
  if (i<strlen(linha)){
    linha[i] = '\0';
  }
  return linha;
}

/**
 * Processa uma linha, palavra por palavra
 *
 * retorna numero da linha se linha invalida
 * retorna 0 linha válida
 */
int processaLinha(char *linha, int numLinha) {
  char delimit[] = " \t";
  int erroPalavra;
  char *palavra;

  eliminaComentarios(linha);  // elimina comentário
  trim(linha);

  palavra = strtok1(linha, delimit);
  while (palavra != NULL) {
    erroPalavra = processaPalavra(palavra, numLinha);

    if (erroPalavra == numLinha) {
      return numLinha;
    }

    trim(linha);
    palavra = strtok1(linha, delimit);
  }
  return 0;
}

/**
 * Transforma as letras de uma palavra em minúsculas
 */
char *toLowerCase(char *entrada) {
  int i;
  for (i = 0; entrada[i] != '\0'; i++) {
    entrada[i] = (char) tolower(entrada[i]);
  }
  return entrada;
}

/**
 * Verifica se ha erro lexico
 * Retorna 0 se tokens válidos
 * Retorna numero da linha invalida se tokens invalidos
 */
int verificaErroGramatical() {
  int i, numberOfTokens = getNumberOfTokens();
  Token atual, prox;
  if (numberOfTokens > 0) {
    for (i = 0; i < numberOfTokens - 1; i++) {
      atual = recuperaToken(i);
      prox = recuperaToken(i + 1);
      switch (atual.tipo) {
        case DefRotulo: {
          if (atual.linha == prox.linha) {
            if (prox.tipo == DefRotulo) {  // Rotulo apos Rotulo na mesma linha
              return prox.linha;
            }
          }
          if (prox.tipo == Hexadecimal || prox.tipo == Decimal ||
              prox.tipo == Nome) {  // Hexadecimal, Decimal ou nome apos Rotulo
            return prox.linha;
          }
          break;
        }
        case Diretiva: {
          if (prox.tipo == DefRotulo || prox.tipo == Diretiva ||
              prox.tipo == Instrucao) {
            // Rotulo, Diretiva ou Instrucao apos Diretiva
            return prox.linha;
          }
          break;
        }
        case Instrucao: {
          if (prox.tipo == DefRotulo || prox.tipo == Diretiva ||
              prox.tipo == Instrucao || prox.tipo == Decimal) {
            // Rotulo, Diretiva ou Instrucao apos Diretiva
            return prox.linha;
          }
          break;
        }
        case Hexadecimal: {
          if (atual.linha == prox.linha) {
            return prox.linha;
          }
          if (prox.tipo == Decimal || prox.tipo == Hexadecimal ||
              prox.tipo == Nome) {
            // Decimal, Hexadecimal ou Nome apos Hexadecimal
            return prox.linha;
          }
          break;
        }
        case Decimal: {
          if (atual.linha == prox.linha) {
            return prox.linha;
          }
          if (prox.tipo == Decimal || prox.tipo == Hexadecimal ||
              prox.tipo == Nome) {
            // Decimal, Hexadecimal ou Nome apos Decimal
            return prox.linha;
          }
          break;
        }
        case Nome: {
          if (atual.linha == prox.linha) {
            return prox.linha;
          }
          if (prox.tipo == Decimal || prox.tipo == Hexadecimal ||
              prox.tipo == Nome) {
            // Decimal, Hexadecimal ou Nome apos Nome
            return prox.linha;
          }
          break;
        }
      }
    }
  }
  return 0;
}

/**
 * Processa o a entrada linha a linha
 * retorna 0 se entrada válida
 * retorna 1 se entrada inválida
 */
int processarEntrada(char *entrada, unsigned tamanho) {
  entrada = toLowerCase(entrada);
  char *linha;
  char delimit[] = "\n";
  int numLinha = 1, erroLexico = 0;

  linha = strtok1(entrada, delimit);
  while (linha != NULL) {
    if (strcmp(linha,"") != 0) {
      erroLexico = processaLinha(linha, numLinha);

      if (erroLexico != 0) {
        printf("ERRO LEXICO: palavra invalida na linha %d", erroLexico);
        return 1;  // erro
      }
    }
    linha = strtok1(entrada, delimit);
    numLinha++;
  }
  int erroGramatical = verificaErroGramatical();
  if (erroGramatical != 0) {
    printf("ERRO GRAMATICAL: palavra invalida na linha %d", erroGramatical);
    return 1;
  }
  /* printf("Você deve implementar esta função para a Parte 1.\n"); */
  return 0;
}