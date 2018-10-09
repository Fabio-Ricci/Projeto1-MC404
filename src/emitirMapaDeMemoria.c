#include "montador.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Aluno: Fábio Camargo Ricci
 * RA: 170781
 * MC404
 */

typedef struct MemoryAddress {
  int memoryAddress;
  int position;
} MemoryAddress;

typedef struct MemoryWord {
  MemoryAddress memoryAddress;
  char instructionCode[3];
  char memoryReference[10];
} MemoryWord;

MemoryWord createWord(unsigned actualAddress, unsigned position, char code[3]) {
  MemoryWord word;
  MemoryAddress endereco;
  endereco.memoryAddress = actualAddress;
  endereco.position = position;
  word.memoryAddress = endereco;
  strcpy(word.instructionCode, code);
  return word;
}

MemoryAddress getEndereco(char **rotulos,
                          MemoryAddress *enderecosRotulos,
                          unsigned tamRotulos,
                          Token *nomes,
                          Token *valoresNomes,
                          unsigned tamNomes,
                          char *palavra) {
  int i;
  char *aux;
  MemoryAddress endereco;
  for (i = 0; i < tamNomes && strcmp(nomes[i].palavra, palavra) != 0;
       i++) {} //  procura pela definicao do nome
  if (i < tamNomes) { // referencia a nome existente
    if (valoresNomes[i].tipo == Decimal) {
      endereco.memoryAddress = strtol(valoresNomes[i].palavra, &aux, 10);
      endereco.position = 0;
    } else if (valoresNomes[i].tipo == Hexadecimal) {
      endereco.memoryAddress = strtol(valoresNomes[i].palavra, &aux, 16);
      endereco.position = 0;
    }
  } else { // procurar no vetor de rotulos
    for (i = 0; i < tamRotulos && strcmp(rotulos[i], palavra) != 0;
         i++) {} // procura pela definicao do rotulo
    if (i < tamRotulos) { // referencia a rotulo existente
      endereco.memoryAddress = strtol(valoresNomes[i].palavra, &aux, 10);
      endereco.position = enderecosRotulos[i].position;
    } else { // rotulo nem nome existente, retornar erro
      endereco.memoryAddress = -1; // simbolizar que houve erro
      endereco.position = -1;
    }
  }
  return endereco;
}

MemoryWord fillNumero(int n, MemoryWord word) {
  char *aux;
  unsigned i, j;
  char ref[11]; // palavra final com todos os zeros
  char num[11]; // string auxiliar para guardar o valor do numero
  sprintf(num, "%X", n); // converte int para string hexa
  for (i = 0; i < (10 - strlen(num)); i++) { // preenche com zeros
    ref[i] = '0';
  }
  for (j = 0; j < strlen(num); j++, i++) { // preenche com o numero
    ref[i] = num[j];
  }
  ref[10] = '\0';
  strcpy(word.memoryReference, ref);
  return word;
}

/**
 * Cria e preenche um MemoryWord de acordo com a instrucao passada
 * retorna -1 em memoryReference se houver erro
 * */
MemoryWord fillMemoryWord(Token atual,
                          Token prox,
                          int actualAddress,
                          int position,
                          int tamNomes,
                          Token *nomes,
                          Token *valoresNomes,
                          int tamRotulos,
                          char **rotulos,
                          MemoryAddress *enderecosRotulos,
                          char code[3]) {
  MemoryWord word = createWord(actualAddress, position, code);
  if (prox.tipo == Decimal) {
    char *aux;
    word = fillNumero(strtol(prox.palavra, &aux, 10), word);
  } else if (prox.tipo == Hexadecimal) {
    char *aux;
    word = fillNumero(strtol(prox.palavra, &aux, 16), word);
  } else if (prox.tipo == Nome) {
    snprintf(word.memoryReference,
             3,
             "%03X",
             getEndereco(rotulos,
                         enderecosRotulos,
                         tamRotulos,
                         nomes,
                         valoresNomes,
                         tamNomes,
                         atual.palavra).memoryAddress);
  }
  return word;
}

char *adicionarWord(char **map, MemoryWord *memoryMap, int i) {
  unsigned j, k;
  char end[4];
  snprintf(end, 4, "%03x", memoryMap[i].memoryAddress.memoryAddress);
  for (j = 0; j < 3; j++) { // preenche o endereco da linha atual
    map[i][j] = end[j];
  }
  map[i][j] = ' ';
  j++;

  for (k = 0; k < 10; j++, k++) {
    map[i][j] = memoryMap[i].memoryReference[k];
    if (k % 2 == 1) {
      j++;
      map[i][j] = ' ';
    }
  }
  map[i][j] = '\0';
}

char **gerarMapa(MemoryWord *memoryMap, int tamMemoryMap) {
  unsigned i, j, k;
  char end[4];
  char **map = malloc((tamMemoryMap / 2) * sizeof(char *)); //tamMemoryMap/2 pois a sao 2 instrucoes por linha
  for (i = 0; i < tamMemoryMap; i++) {
    map[i] = malloc(19 * sizeof(char *));
    if (strcmp(memoryMap[i].instructionCode, "99") == 0) { //.word
      adicionarWord(map, memoryMap, i);
    } else { // instrucao

    }
  }
  if (i % 2 == 1) {
    // preecher o resto da linha com zero
  }
  return map;
}

int emitirMapaDeMemoria() {
  unsigned i;
  int actualAddress = 0x000, numberOfTokens = getNumberOfTokens(),
      position = 0, indRotulos = 0, tamRotulos = 0, capRotulos = 2, indNomes = 0, tamNomes = 0, capNomes = 2,
      tamMemoryMap = 0;

  // tabela de rotulos
  char **rotulos = malloc(capRotulos * sizeof(char *)); // nomes dos rotulos
  MemoryAddress *enderecosRotulos = malloc(capRotulos * sizeof(MemoryAddress)); // endereco dos rotulos

  Token *nomes = malloc(capNomes * sizeof(Token)); // vetor de nomes definidos
  Token *valoresNomes = malloc(capNomes * sizeof(Token)); // valores dos nomes definidos

  Token atual, prox;

  MemoryWord *memoryMap = malloc(numberOfTokens * sizeof(MemoryWord)); // mapa de memoria final
  MemoryWord word;

  /***********************************************************************************************/
  for (i = 0; i < numberOfTokens; i++) { // leitura dos rotulos e .sets
    atual = recuperaToken(i);
    if (atual.tipo == DefRotulo) {
      if (indRotulos == capRotulos) {
        capRotulos *= 2;
        enderecosRotulos = realloc(enderecosRotulos, capRotulos * sizeof(MemoryAddress));
        rotulos = realloc(rotulos, capRotulos * sizeof(char *));
      }
      // endereco do rotulo
      MemoryAddress enderecoRotulo;
      enderecoRotulo.memoryAddress = actualAddress;
      enderecoRotulo.position = position;
      enderecosRotulos[indRotulos] = enderecoRotulo;

      // nome do rotulo
      rotulos[indRotulos] = malloc((strlen(atual.palavra) + 1) * sizeof(char *));
      strcpy(rotulos[indRotulos], atual.palavra);

      tamRotulos++;
      indRotulos++;
    } else if (strcmp(atual.palavra, ".set") == 0) {
      if (indNomes == capNomes) {
        capNomes *= 2;
        nomes = realloc(nomes, capNomes * sizeof(char *));
        valoresNomes = realloc(valoresNomes, capNomes * sizeof(char *));
      }
      i++;
      atual = recuperaToken(i); // pega o nome
      nomes[indNomes] = atual;

      i++;
      atual = recuperaToken(i); // pega o valor do nome
      valoresNomes[indNomes] = atual;

      indNomes++;
      tamNomes++;
    }
  }
  /******************************************************************************************************/

  for (i = 0; i < numberOfTokens; i++) { //  geracao mapa de memoria
    atual = recuperaToken(i);
    if (i + 1 < numberOfTokens) {
      prox = recuperaToken(i + 1);
    }

    switch (atual.tipo) {
      case Diretiva: {  // .set ja foi feito na seccao de preenchimento dos nomes e rotulos
        if (strcmp(atual.palavra, ".org") == 0) {
          char *aux;
          if (prox.tipo == Decimal) {
            actualAddress = (int) strtol(prox.palavra, &aux, 10);
          } else if (prox.tipo == Hexadecimal) {
            actualAddress = (int) strtol(prox.palavra, &aux, 16);
          }
          position = 0;
        } else if (strcmp(atual.palavra, ".align") == 0) {
          char *aux;
          int multiplo = strtol(prox.palavra, &aux, 10);
          for (; actualAddress % multiplo == 0; actualAddress++) {
            // talvez adicionar instrucoes sem efeito
          }
        } else if (strcmp(atual.palavra, ".wfill") == 0) {
          if (position == 1) { // erro
            strcpy(word.memoryReference, "-1");
          } else {
            char *aux;
            int j, n = strtol(prox.palavra, &aux, 10);
            i++;
            atual = prox;
            prox = recuperaToken(i + 1);
            for (j = 0; j < n; j++, actualAddress++) {
              word = fillMemoryWord(atual,
                                    prox,
                                    actualAddress,
                                    position,
                                    tamNomes,
                                    nomes,
                                    valoresNomes,
                                    tamRotulos,
                                    rotulos,
                                    enderecosRotulos,
                                    "99"); // 99 para simbolizar que é um .word ou .wfill
              memoryMap[tamMemoryMap] = word;
              tamMemoryMap++;
              actualAddress++;
            }
          }
        } else if (strcmp(atual.palavra, ".word") == 0) {
          if (position == 1) { // erro
            strcpy(word.memoryReference, "-1");
          } else {
            word = fillMemoryWord(atual,
                                  prox,
                                  actualAddress,
                                  position,
                                  tamNomes,
                                  nomes,
                                  valoresNomes,
                                  tamRotulos,
                                  rotulos,
                                  enderecosRotulos,
                                  "99"); // 99 para simbolizar que é um .word ou .wfill
            memoryMap[tamMemoryMap] = word;
            tamMemoryMap++;
            actualAddress++;
          }
        }
        break;
      }
      case Instrucao: {
        char *intrucoes[19] = {"ld", "ldinv", "ldabs", "ldmq", "ldmqmx",
                               "store", "jump", "jumpl", "jumpr", "add",
                               "addabs", "sub", "subabs", "mult", "div",
                               "lsh", "rsh", "storal", "storar"};
        if (strcmp(atual.palavra, "ld") == 0) {
          word = fillMemoryWord(atual,
                                prox,
                                actualAddress,
                                position,
                                tamNomes,
                                nomes,
                                valoresNomes,
                                tamRotulos,
                                rotulos,
                                enderecosRotulos,
                                "01");
          memoryMap[tamMemoryMap] = word;
          tamMemoryMap++;
        } else if (strcmp(atual.palavra, "ldinv") == 0) {
          word = fillMemoryWord(atual,
                                prox,
                                actualAddress,
                                position,
                                tamNomes,
                                nomes,
                                valoresNomes,
                                tamRotulos,
                                rotulos,
                                enderecosRotulos,
                                "02");
          memoryMap[tamMemoryMap] = word;
          tamMemoryMap++;
        } else if (strcmp(atual.palavra, "ldabs") == 0) {
          word = fillMemoryWord(atual,
                                prox,
                                actualAddress,
                                position,
                                tamNomes,
                                nomes,
                                valoresNomes,
                                tamRotulos,
                                rotulos,
                                enderecosRotulos,
                                "03");
          memoryMap[tamMemoryMap] = word;
          tamMemoryMap++;
        } else if (strcmp(atual.palavra, "ldmq") == 0) {
          word = createWord(actualAddress, position, "10");
          strcpy(word.memoryReference, "00");

          memoryMap[tamMemoryMap] = word;
          tamMemoryMap++;
        } else if (strcmp(atual.palavra, "ldmgmx") == 0) {
          word = fillMemoryWord(atual,
                                prox,
                                actualAddress,
                                position,
                                tamNomes,
                                nomes,
                                valoresNomes,
                                tamRotulos,
                                rotulos,
                                enderecosRotulos,
                                "09");
          memoryMap[tamMemoryMap] = word;
          tamMemoryMap++;
        } else if (strcmp(atual.palavra, "store") == 0) {
          word = fillMemoryWord(atual,
                                prox,
                                actualAddress,
                                position,
                                tamNomes,
                                nomes,
                                valoresNomes,
                                tamRotulos,
                                rotulos,
                                enderecosRotulos,
                                "21");
          memoryMap[tamMemoryMap] = word;
          tamMemoryMap++;
        } else if (strcmp(atual.palavra, "jump") == 0) {
          char *aux;
          if (prox.tipo == Decimal) {
            word = createWord(actualAddress, position, "0D"); // pula para a esquerda
            strcpy(word.memoryReference, prox.palavra);
          } else if (prox.tipo == Hexadecimal) {
            word = createWord(actualAddress, position, "0D"); // pula para a esquerda
            strcpy(word.memoryReference, prox.palavra);
          } else if (prox.tipo == Nome) {
            MemoryAddress
                add = getEndereco(rotulos, enderecosRotulos, tamRotulos, nomes, valoresNomes, tamNomes, prox.palavra);
            if (add.position == 0) {
              word = createWord(actualAddress, position, "0D"); // pula para a esquerda
            } else if (add.position == 1) {
              word = createWord(actualAddress, position, "0E"); // pula para a direita
            }
            char aux[4];
            snprintf(aux, 3, "%03x", add.memoryAddress);
            strcpy(word.memoryReference, aux);
          }
        } else if (strcmp(atual.palavra, "jumpl") == 0) {
          word = createWord(actualAddress, position, "0F"); // pulo condicional para a esquerda
          if (prox.tipo == Decimal) {
            strcpy(word.memoryReference, prox.palavra);
          } else if (prox.tipo == Hexadecimal) {
            strcpy(word.memoryReference, prox.palavra);
          } else if (prox.tipo == Nome) {
            MemoryAddress
                add = getEndereco(rotulos, enderecosRotulos, tamRotulos, nomes, valoresNomes, tamNomes, prox.palavra);
            char aux[4];
            snprintf(aux, 3, "%03x", add.memoryAddress);
            strcpy(word.memoryReference, aux);
          }
        } else if (strcmp(atual.palavra, "jumpr") == 0) {
          word = createWord(actualAddress, position, "10"); // pulo condicional para a direita
          if (prox.tipo == Decimal) {
            strcpy(word.memoryReference, prox.palavra);
          } else if (prox.tipo == Hexadecimal) {
            strcpy(word.memoryReference, prox.palavra);
          } else if (prox.tipo == Nome) {
            MemoryAddress
                add = getEndereco(rotulos, enderecosRotulos, tamRotulos, nomes, valoresNomes, tamNomes, prox.palavra);
            char aux[4];
            snprintf(aux, 3, "%03x", add.memoryAddress);
            strcpy(word.memoryReference, aux);
          }
        } else if (strcmp(atual.palavra, "add") == 0) {
          word = fillMemoryWord(atual,
                                prox,
                                actualAddress,
                                position,
                                tamNomes,
                                nomes,
                                valoresNomes,
                                tamRotulos,
                                rotulos,
                                enderecosRotulos,
                                "05");
          memoryMap[tamMemoryMap] = word;
          tamMemoryMap++;
        } else if (strcmp(atual.palavra, "addabs") == 0) {
          word = fillMemoryWord(atual,
                                prox,
                                actualAddress,
                                position,
                                tamNomes,
                                nomes,
                                valoresNomes,
                                tamRotulos,
                                rotulos,
                                enderecosRotulos,
                                "07");
          memoryMap[tamMemoryMap] = word;
          tamMemoryMap++;
        } else if (strcmp(atual.palavra, "sub") == 0) {
          word = fillMemoryWord(atual,
                                prox,
                                actualAddress,
                                position,
                                tamNomes,
                                nomes,
                                valoresNomes,
                                tamRotulos,
                                rotulos,
                                enderecosRotulos,
                                "06");
          memoryMap[tamMemoryMap] = word;
          tamMemoryMap++;
        } else if (strcmp(atual.palavra, "subabs") == 0) {
          word = fillMemoryWord(atual,
                                prox,
                                actualAddress,
                                position,
                                tamNomes,
                                nomes,
                                valoresNomes,
                                tamRotulos,
                                rotulos,
                                enderecosRotulos,
                                "08");
          memoryMap[tamMemoryMap] = word;
          tamMemoryMap++;
        } else if (strcmp(atual.palavra, "mult") == 0) {
          word = fillMemoryWord(atual,
                                prox,
                                actualAddress,
                                position,
                                tamNomes,
                                nomes,
                                valoresNomes,
                                tamRotulos,
                                rotulos,
                                enderecosRotulos,
                                "0B");
          memoryMap[tamMemoryMap] = word;
          tamMemoryMap++;
        } else if (strcmp(atual.palavra, "div") == 0) {
          word = fillMemoryWord(atual,
                                prox,
                                actualAddress,
                                position,
                                tamNomes,
                                nomes,
                                valoresNomes,
                                tamRotulos,
                                rotulos,
                                enderecosRotulos,
                                "0C");
          memoryMap[tamMemoryMap] = word;
          tamMemoryMap++;
        } else if (strcmp(atual.palavra, "lsh") == 0) {
          word = createWord(actualAddress, position, "14");
          strcpy(word.memoryReference, "00");

          memoryMap[tamMemoryMap] = word;
          tamMemoryMap++;
        } else if (strcmp(atual.palavra, "rsh") == 0) {
          word = createWord(actualAddress, position, "15");
          strcpy(word.memoryReference, "00");

          memoryMap[tamMemoryMap] = word;
          tamMemoryMap++;
        } else if (strcmp(atual.palavra, "storal") == 0) {
          word = createWord(actualAddress, position, "12");
          if (prox.tipo == Decimal) {
            strcpy(word.memoryReference, prox.palavra);
          } else if (prox.tipo == Hexadecimal) {
            strcpy(word.memoryReference, prox.palavra);
          } else if (prox.tipo == Nome) {
            MemoryAddress
                add = getEndereco(rotulos, enderecosRotulos, tamRotulos, nomes, valoresNomes, tamNomes, prox.palavra);
            char aux[4];
            snprintf(aux, 3, "%03x", add.memoryAddress);
            strcpy(word.memoryReference, aux);
          }
          memoryMap[tamMemoryMap] = word;
          tamMemoryMap++;
        } else if (strcmp(atual.palavra, "storar") == 0) {
          word = createWord(actualAddress, position, "13");
          if (prox.tipo == Decimal) {
            strcpy(word.memoryReference, prox.palavra);
          } else if (prox.tipo == Hexadecimal) {
            strcpy(word.memoryReference, prox.palavra);
          } else if (prox.tipo == Nome) {
            MemoryAddress
                add = getEndereco(rotulos, enderecosRotulos, tamRotulos, nomes, valoresNomes, tamNomes, prox.palavra);
            char aux[4];
            snprintf(aux, 3, "%03x", add.memoryAddress);
            strcpy(word.memoryReference, aux);
          }
          memoryMap[tamMemoryMap] = word;
          tamMemoryMap++;
        }
        if (position == 1) {
          actualAddress++;
        }
        position = !position;
        break;
      }
    }

    if (strcmp(word.memoryReference, "-1") == 0) { // codigo definida para erro de nao definido
      fprintf(stderr,
              "ERRO: Usado mas não definido: %s!",
              prox.palavra); // prox contera o nome do rotulo ou nome referenciado
      return 1;
    }
  }
  char **map = gerarMapa(memoryMap, tamMemoryMap);
  for (i = 0; i < tamMemoryMap; i++) {
    printf("%s\n", map[i]);
  }
  return 0;
}