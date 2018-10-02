#include "montador.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
    ---- Voce deve implementar essa função para a Parte 2! ----
    Utilize os tokens da estrutura de tokens
    Retorna:
        *  1 caso haja erro na montagem;
        *  0 caso não haja erro.
 */

typedef struct MemoryAddress {
  int memoryAddress;
  int position;
} MemoryAddress;

typedef struct MemoryWord {
  MemoryAddress memoryAddress;
  int instructionCode;
  int memoryReference;
} MemoryWord;

int lerTokens() {
}

// preenche os vetores de nomes e rotulos
int preencherRotulosNomes(MemoryAddress *enderecosRotulos, char **rotulos, char **nomes, int *valoresNomes) {
  unsigned i, actualAddress = 0x000, numberOfTokens = getNumberOfTokens(),
      position = 0, indRotulos = 0, tamRotulos = 2, indNomes = 0, tamNomes = 2;
  Token atual;
  for (i = 0; i < numberOfTokens; i++) { // leitura dos rotulos e .sets
    atual = recuperaToken(i);
    if (atual.tipo == DefRotulo) {
      if (indRotulos == tamRotulos) {
        tamRotulos *= 2;
        enderecosRotulos = realloc(enderecosRotulos, tamRotulos * sizeof(MemoryAddress));
        rotulos = realloc(rotulos, tamRotulos * sizeof(char *));
      }
      // endereco do rotulo
      MemoryAddress enderecoRotulo;
      enderecoRotulo.memoryAddress = actualAddress;
      enderecoRotulo.position = position;
      enderecosRotulos[indRotulos] = enderecoRotulo;

      // nome do rotulo
      rotulos[indRotulos] = malloc((strlen(atual.palavra) + 1) * sizeof(char *));
      strcpy(rotulos[indRotulos], atual.palavra);

      indRotulos++;
    } else if (strcmp(atual.palavra, ".set") == 0) {
      if (indNomes == tamNomes) {
        tamNomes *= 2;
        nomes = realloc(nomes, tamNomes * sizeof(char *));
        valoresNomes = realloc(valoresNomes, tamNomes * sizeof(char *));
      }
      i++;
      atual = recuperaToken(i); // pega o nome
      nomes[indNomes] = malloc((strlen(atual.palavra) + 1) * sizeof(char *));
      strcpy(nomes[indNomes], atual.palavra);

      i++;
      atual = recuperaToken(i); // pega o valor do nome

      char *aux;
      if (atual.tipo == Decimal) {
        valoresNomes[indNomes] = strtol(atual.palavra, &aux, 10);
      } else if (atual.tipo == Hexadecimal) {
        valoresNomes[indNomes] = strtol(atual.palavra, &aux, 16);
      }
      indNomes++;
    }
  }
}

int emitirMapaDeMemoria() {
  unsigned i, actualAddress = 0x000, numberOfTokens = getNumberOfTokens(),
      position = 0, indRotulos = 0, tamRotulos = 2, indNomes = 0, tamNomes = 2, indMemoryMap = 0;

  // tabela de rotulos
  MemoryAddress *enderecosRotulos = malloc(tamRotulos * sizeof(MemoryAddress)); // endereco dos rotulos
  char **rotulos = malloc(tamRotulos * sizeof(char *)); // nomes dos rotulos

  char **nomes = malloc(tamNomes * sizeof(char *)); // vetor de nomes definidos
  int *valoresNomes = malloc(tamNomes * sizeof(int)); // valores dos nomes definidos

  Token atual, prox;

  MemoryWord *memoryMap = malloc(numberOfTokens * sizeof(MemoryWord)); // mapa de memoria final
  MemoryWord word;

  preencherRotulosNomes(enderecosRotulos, rotulos, nomes, valoresNomes);

  for (i = 0; i < numberOfTokens; i++) { //  geracao mapa de memoria
    atual = recuperaToken(i);
    if (i + 1 < numberOfTokens) {
      prox = recuperaToken(i + 1);
    }

    switch (atual.tipo) {
      case Diretiva: {
        if (strcmp(atual.palavra, ".org") == 0) {
          char *aux;
          if (prox.tipo == Decimal) {
            actualAddress = strtol(prox.palavra, &aux, 10);
          } else if (prox.tipo == Hexadecimal) {
            actualAddress = strtol(prox.palavra, &aux, 16);
          }
          position = 0;
        } else if (strcmp(atual.palavra, ".align") == 0) {
          char *aux;
          int multiplo = strtol(prox.palavra, &aux, 10);
          for (; actualAddress % multiplo == 0; actualAddress++) {}
        } else if (strcmp(atual.palavra, ".set") == 0) {
        } else if (strcmp(atual.palavra, ".wfill") == 0) {
          char *aux;
          if (position == 1) { // ajusta a posicao de memoria
            actualAddress++;
            position = !position;
          }

          int ind, n = strtol(prox.palavra, &aux, 10);
          MemoryAddress endereco;
          i++;
          atual = prox;
          prox = recuperaToken(i + 1);

          for (ind = 0; ind < n; ind++, actualAddress++) {
            endereco.memoryAddress = actualAddress;
            endereco.position = position;
            word.memoryAddress = endereco;
            word.instructionCode = 00; // convencao para indicar que eh um .word
            if (prox.tipo == Decimal) {
              word.memoryReference = strtol(prox.palavra, &aux, 10);
            } else if (prox.tipo == Hexadecimal) {
              word.memoryReference = strtol(prox.palavra, &aux, 16);
            } else if (prox.tipo == Nome) {
            }
            memoryMap[indMemoryMap] = word;
            indMemoryMap++;
          }
        } else if (strcmp(atual.palavra, ".word") == 0) {
        }
        break;
      }
      case Instrucao: {
        char *intrucoes[19] = {"ld", "ldinv", "ldabs", "ldmq", "ldmqmx",
                               "store", "jump", "jumpl", "jumpr", "add",
                               "addabs", "sub", "subabs", "mult", "div",
                               "lsh", "rsh", "storal", "storar"};
        if (strcmp(atual.palavra, "ld") == 0) {
          MemoryAddress endereco;
          endereco.memoryAddress = actualAddress;
          endereco.position = position;
          word.memoryAddress = endereco;
          word.instructionCode = 01;

          char *aux;
          if (prox.tipo == Decimal) {
            word.memoryReference = strtol(prox.palavra, &aux, 10);
          } else if (prox.tipo == Hexadecimal) {
            word.memoryReference = strtol(prox.palavra, &aux, 16);
          } else if (prox.tipo == Nome) {
          }

          memoryMap[indMemoryMap] = word;
          indMemoryMap++;
        } else if (strcmp(atual.palavra, "ldinv") == 0) {
        } else if (strcmp(atual.palavra, "ldabs") == 0) {
        } else if (strcmp(atual.palavra, "ldmg") == 0) {
        } else if (strcmp(atual.palavra, "ldmgmx") == 0) {
        } else if (strcmp(atual.palavra, "store") == 0) {
        } else if (strcmp(atual.palavra, "jump") == 0) {
        } else if (strcmp(atual.palavra, "jumpl") == 0) {
        } else if (strcmp(atual.palavra, "jumpr") == 0) {
        } else if (strcmp(atual.palavra, "add") == 0) {
        } else if (strcmp(atual.palavra, "addabs") == 0) {
        } else if (strcmp(atual.palavra, "sub") == 0) {
        } else if (strcmp(atual.palavra, "subabs") == 0) {
        } else if (strcmp(atual.palavra, "mult") == 0) {
        } else if (strcmp(atual.palavra, "div") == 0) {
        } else if (strcmp(atual.palavra, "lsh") == 0) {
        } else if (strcmp(atual.palavra, "rsh") == 0) {
        } else if (strcmp(atual.palavra, "storal") == 0) {
        } else if (strcmp(atual.palavra, "storar") == 0) {
        }
        break;
      }
    }
    if (position == 1) {
      actualAddress++;
    }
    position = !position;
  }
/* printf("Voce deve implementar essa função para a Parte 2!");*/
  return 0;
}
