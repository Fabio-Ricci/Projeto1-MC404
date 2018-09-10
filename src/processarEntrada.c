#include <stdio.h>
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
void criaToken(char *palavra, TipoDoToken tipo, int numLinha){
  Token token;
  token.tipo = tipo;
  token.linha = numLinha;
  strcpy(token.palavra, palavra);
  adicionarToken(token);
}

/**
 * Processa uma palavra
 * retorna ERROR_OFFSET (-1000) se palavra invalida
 * retorna 0 se palavra válida
 */
int processaPalavra(char *palavra, int numLinha) {
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