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
  int instructionCode;
  int memoryReference;
} MemoryWord;

MemoryWord createWord(unsigned actualAddress, unsigned position, int code) {
  MemoryWord word;
  MemoryAddress endereco;
  endereco.memoryAddress = actualAddress;
  endereco.position = position;
  word.memoryAddress = endereco;
  word.instructionCode = code;
  return word;
}

/**
 * Verfica qual tipo de operacao eh e efetua se nao ha erro
 * */
int findMultiplier(int code, int number) {
  switch (code) {
    case 1: { // LOAD M(X)
      break;
    }
    case 2: { // LOAD -M(X)
      number = (-1) * number;
      break;
    }
    case 3: { // LOAD |M(X)|
      number = abs(number);
      break;
    }
    case 5: { // ADD M(X)
      break;
    }
    case 6: { // SUB M(X)
      break;
    }
    case 7: { // ADDABS M(X)
      number = abs(number);
      break;
    }
    case 8: { // SUBABS M(X)
      number = abs(number);
      break;
    }
    case 9: { // LOAD MQ, M(X)
      break;
    }
    case 11: { // MUL M(X)
      break;
    }
    case 12: { // DIV M(X)
      break;
    }
    case 21: { // STOR M(X)
      break;
    }
    default: {
      break;
    }
  }
  return number;
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
                          int code) {
  char *aux;
  MemoryWord word = createWord(actualAddress, position, code);

  if (prox.tipo == Decimal) {
    word.memoryReference = findMultiplier(code, strtol(prox.palavra, &aux, 10));
  } else if (prox.tipo == Hexadecimal) {
    word.memoryReference = findMultiplier(code, strtol(prox.palavra, &aux, 16));
  } else if (prox.tipo == Nome) {
    word.memoryReference =
        getEndereco(rotulos, enderecosRotulos, tamRotulos, nomes, valoresNomes, tamNomes, atual.palavra).memoryAddress;
  }
  return word;
}

int getNumberOfDigits(int n) {
  int count = 0;
  while (n != 0) {
    n /= 10;
    count++;
  }
  return count;
}

char **gerarMapa(MemoryWord *memoryMap, int tamMemoryMap) {
  unsigned i, j = 0;
  char numLinha[4], code[3], end[4];
  char **map = malloc((tamMemoryMap / 2) * sizeof(char *)); //tamMemoryMap/2 pois a sao 2 instrucoes por linha
  for (i = 0; i < tamMemoryMap; i++) {
    if (memoryMap[i].instructionCode == 0) { //.word
      map[i] = malloc(19 * sizeof(char)); // 19 - numero de caracteres por linha no formato desejado
      snprintf(numLinha,
               3,
               "%d",
               memoryMap[i].memoryAddress.memoryAddress); // tranforma o end atual de int para string
      for (j = 0; j < (3 - strlen(numLinha)); j++) { // preenche o end atual na string final
        map[i][j] = '0';
      }
      unsigned ind;
      for (ind = 0; j < 3; j++, ind++) { // preenche o end atual na string final
        map[i][j] = numLinha[ind];
      }
      j++;
      map[i][j] = ' ';
      j++;

      int numberOfDigits = getNumberOfDigits(memoryMap[i].memoryReference);
      char num[numberOfDigits + 1]; // parametro do .word
      snprintf(num,
               numberOfDigits,
               "%d",
               memoryMap[i].memoryReference);
      unsigned u, k;
      for (u = 0, k = 0; u < (11 - numberOfDigits); u++, j++) { // preenche com zeros
        map[i][j] = '0';
        if (k % 2 == 1) {
          k = 0;
          j++;
          map[i][j] = ' ';
        } else {
          k++;
        }
      }
      for (u = 0; u < numberOfDigits; u++, k++, j++) { // preenche com o parametro do .word
        map[i][j] = num[u];
        if (k % 2 == 1) {
          k = 0;
          j++;
          map[i][j] = ' ';
        }
      }
      map[i][j] = '\0';
    } else { // instrucao normal
      if (memoryMap[i].memoryAddress.position == 0) { // primeira palavra da linha
        map[i] = malloc(19 * sizeof(char)); // 19 - numero de caracteres por linha no formato desejado
        snprintf(numLinha,
                 3,
                 "%d",
                 memoryMap[i].memoryAddress.memoryAddress); // tranforma o end atual de int para string
        for (j = 0; j < 3; j++) { // preenche o end atual na string final
          map[i][j] = numLinha[j];
        }
        j++;
        map[i][j] = ' ';
        j++;

        snprintf(code, 2, "%d", memoryMap[i].instructionCode); // tranforma o codigo de int para string
        map[i][j] = code[0];
        j++;
        map[i][j] = code[1];
        j++;
        map[i][j] = ' ';
        j++;

        snprintf(end,
                 3,
                 "%d",
                 memoryMap[i].memoryReference); // tranforma o end de memoria referenciado de int para string
        map[i][j] = end[0];
        j++;
        map[i][j] = end[1];
        j++;
        map[i][j] = ' ';
        j++;
        map[i][j] = end[2];
        j++;
      } else { // segunda palavra da linha
        snprintf(code, 2, "%d", memoryMap[i].instructionCode); // tranforma o codigo de int para string
        map[i][j] = code[0];
        j++;
        map[i][j] = ' ';
        j++;
        map[i][j] = code[1];
        j++;

        snprintf(end,
                 3,
                 "%d",
                 memoryMap[i].memoryReference); // tranforma o end de memoria referenciado de int para string
        map[i][j] = end[0];
        j++;
        map[i][j] = ' ';
        j++;
        map[i][j] = end[1];
        j++;
        map[i][j] = end[2];
        j++;

        map[i][j] = '\0';
      }
    }
  }
  if (i % 2 != 0) {
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
          char *aux;
          if (position == 1) { // ajusta a posicao de memoria
            actualAddress++;
            position = !position;
          }

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
                                  0);
            memoryMap[tamMemoryMap] = word;
            tamMemoryMap++;
            actualAddress++;
          }
        } else if (strcmp(atual.palavra, ".word") == 0) {
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
                                0);
          memoryMap[tamMemoryMap] = word;
          tamMemoryMap++;
          actualAddress++;
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
                                0x001);
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
                                0x002);
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
                                0x003);
          memoryMap[tamMemoryMap] = word;
          tamMemoryMap++;
        } else if (strcmp(atual.palavra, "ldmq") == 0) {
          word = createWord(actualAddress, position, 10);
          word.memoryReference = 0x000;

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
                                0x009);
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
                                0x021);
          memoryMap[tamMemoryMap] = word;
          tamMemoryMap++;
        } else if (strcmp(atual.palavra, "jump") == 0) {
          char *aux;
          if (prox.tipo == Decimal) {
            word = createWord(actualAddress, position, 0x00D); // pula para a esquerda
            word.memoryReference = strtol(prox.palavra, &aux, 10);
          } else if (prox.tipo == Hexadecimal) {
            word = createWord(actualAddress, position, 0x00D); // pula para a esquerda
            word.memoryReference = strtol(prox.palavra, &aux, 16);
          } else if (prox.tipo == Nome) {
            MemoryAddress
                add = getEndereco(rotulos, enderecosRotulos, tamRotulos, nomes, valoresNomes, tamNomes, prox.palavra);
            if (add.position == 0) {
              word = createWord(actualAddress, position, 0x00D); // pula para a esquerda
            } else if (add.position == 1) {
              word = createWord(actualAddress, position, 0x00E); // pula para a direita
            }
            word.memoryReference = add.memoryAddress;
          }
        } else if (strcmp(atual.palavra, "jumpl") == 0) {
          char *aux;
          word = createWord(actualAddress, position, 0x00F); // pulo condicional para a esquerda
          if (prox.tipo == Decimal) {
            word.memoryReference = strtol(prox.palavra, &aux, 10);
          } else if (prox.tipo == Hexadecimal) {
            word.memoryReference = strtol(prox.palavra, &aux, 16);
          } else if (prox.tipo == Nome) {
            word.memoryReference = getEndereco(rotulos,
                                               enderecosRotulos,
                                               tamRotulos,
                                               nomes,
                                               valoresNomes,
                                               tamNomes,
                                               prox.palavra).memoryAddress;
          }
        } else if (strcmp(atual.palavra, "jumpr") == 0) {
          char *aux;
          word = createWord(actualAddress, position, 0x010); // pulo condicional para a direita
          if (prox.tipo == Decimal) {
            word.memoryReference = strtol(prox.palavra, &aux, 10);
          } else if (prox.tipo == Hexadecimal) {
            word.memoryReference = strtol(prox.palavra, &aux, 16);
          } else if (prox.tipo == Nome) {
            word.memoryReference = getEndereco(rotulos,
                                               enderecosRotulos,
                                               tamRotulos,
                                               nomes,
                                               valoresNomes,
                                               tamNomes,
                                               prox.palavra).memoryAddress;
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
                                0x005);
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
                                0x007);
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
                                0x006);
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
                                0x008);
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
                                0x00B);
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
                                0x00C);
          memoryMap[tamMemoryMap] = word;
          tamMemoryMap++;
        } else if (strcmp(atual.palavra, "lsh") == 0) {
          word = createWord(actualAddress, position, 0x014);
          word.memoryReference = 0x000;

          memoryMap[tamMemoryMap] = word;
          tamMemoryMap++;
        } else if (strcmp(atual.palavra, "rsh") == 0) {
          word = createWord(actualAddress, position, 0x015);
          word.memoryReference = 0x000;

          memoryMap[tamMemoryMap] = word;
          tamMemoryMap++;
        } else if (strcmp(atual.palavra, "storal") == 0) {
          char *aux;
          word = createWord(actualAddress, position, 0x012);
          if (prox.tipo == Decimal) {
            word.memoryReference = strtol(prox.palavra, &aux, 10);
          } else if (prox.tipo == Hexadecimal) {
            word.memoryReference = strtol(prox.palavra, &aux, 16);
          } else if (prox.tipo == Nome) {
            word.memoryReference = getEndereco(rotulos,
                                               enderecosRotulos,
                                               tamRotulos,
                                               nomes,
                                               valoresNomes,
                                               tamNomes,
                                               prox.palavra).memoryAddress;
          }
          memoryMap[tamMemoryMap] = word;
          tamMemoryMap++;
        } else if (strcmp(atual.palavra, "storar") == 0) {
          char *aux;
          word = createWord(actualAddress, position, 0x013);
          if (prox.tipo == Decimal) {
            word.memoryReference = strtol(prox.palavra, &aux, 10);
          } else if (prox.tipo == Hexadecimal) {
            word.memoryReference = strtol(prox.palavra, &aux, 16);
          } else if (prox.tipo == Nome) {
            word.memoryReference = getEndereco(rotulos,
                                               enderecosRotulos,
                                               tamRotulos,
                                               nomes,
                                               valoresNomes,
                                               tamNomes,
                                               prox.palavra).memoryAddress;
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

    if (word.memoryReference == -1) { // codigo definida para erro de nao definido
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