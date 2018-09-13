#include <ctype.h>
#include <stdio.h>
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

/**
 * Cria um token e coloca no vetor de token
 */
void criaToken(char *palavra, TipoDoToken tipo, int numLinha) {
  Token token;
  token.tipo = tipo;
  token.linha = numLinha;
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
    if (palavra[0] == '.') {
      return 0;  // rótulo inválido comecando com .
    }
    return 1;
  }
  return 0;
}

int ehDiretiva(char *palavra) {
  if (strcmp(palavra, ".org") || strcmp(palavra, ".align") ||
      strcmp(palavra, ".wfill") || strcmp(palavra, ".set") ||
      strcmp(palavra, ".word")) {
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
    if (strcmp(palavra, intrucoes[i])) {  // verifica se eh alguma das intrucoes
      return 1;
    }
  }
  return 0;
}

int ehHexadecimal(char *palavra) {
  if (strlen(palavra) == 10) {
    if (palavra[0] == '0' && palavra[1] == 'x') {
      int i;
      for (i = 2; i < 10; i++) {
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
  if (strlen(palavra) == 8) {
    int i;
    for (i = 0; i < 8; i++) {
      if (!isdigit(palavra[i])) {  // verifica se todos os digitos sao numeros
        return 0;
      }
    }
    return 1;
  }
  return 0;
}

int ehNome(char *palavra) {
  return 0;
}

/**
 * Processa uma palavra
 * retorna ERROR_OFFSET (-1000) se palavra invalida
 * retorna 0 se palavra válida
 */
int processaPalavra(char *palavra, int numLinha) {
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
  return ERROR_OFFSET;
}

char *eliminaComentarios(char *linha) { return strtok(linha, '#'); }

/**
 * Processa uma linha, palavra por palavra
 *
 * retorna ERROR_OFFSET (-1000) se linha invalida
 * retorna 0 linha válida
 */
int processaLinha(char *linha, int numLinha) {
  linha = eliminaComentarios(linha);  // elimina comentário
  char delimit[] = " \t";
  int achouRotulo = 0, achouDiretiva = 0, tipoPalavra;

  char *palavra = strtok(linha, delimit);
  while (palavra != NULL) {
    tipoPalavra = processaPalavra(palavra, numLinha);

    if (tipoPalavra == ERROR_OFFSET) {
      return ERROR_OFFSET;
    }

    palavra = strtok(NULL, delimit);
  }
  return 0;
}

/**
 * Transforma as letras de uma palavra em minúsculas
 */
char *toLowerCase(char *entrada) {
  int i;
  for (i = 0; entrada[i] != '\0'; i++) {
    entrada[i] = toLower(entrada[i]);
  }
  return entrada;
}

/**
 * Processa o a entrada linha a linha
 * retorna 0 se entrada válida
 * retorna 1 se entrada inválida
 */
int processarEntrada(char *entrada, unsigned tamanho) {
  entrada = toLowerCase(entrada);
  char delimit[] = "\n";

  Token token;
  int numLinha = 1, tipoLinha;
  char *linha = strtok(entrada, delimit);
  while (linha != NULL) {
    tipoLinha = processaLinha(linha, numLinha);
    numLinha++;

    if (tipoLinha == ERROR_OFFSET) {
      return 1;  // erro
    }

    linha = strtok(NULL, delimit);
  }
  /* printf("Você deve implementar esta função para a Parte 1.\n"); */
  return 0;
}