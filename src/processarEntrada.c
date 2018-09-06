#include "montador.h"
#include <stdio.h>

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
int ehRotulo(char* palavra){
    return 0;
}

int ehDiretiva(char* palavra){
    return 0;
}

int ehInstrucao(char* palavra){
    return 0;
}

int ehHexadecimal(char* palavra){
    return 0;
}

int ehDecimal(char* palavra){
    return 0;
}

int ehNome(char* palavra){
    return 0;
}

char *eliminaComentarios(char *entrada){
    return strtok(entrada, '#');
}

/**
 * splita a linha em tokens e coloca no vetor de tokens
 *
 * retorna 0 se linha válida
 * retorna 1 se linha inválida*/
int processaLinha(char *entrada){
    entrada = eliminaComentarios(entrada); //elimina comentário
    char delimit[] = " \t";
    int achouRotulo = 0, achouDiretiva = 0;
    int ehRotulo = 0, ehDiretiva = 0, ehInstrucao, ehHexadecimal, ehDecimal = 0, ehNome = 0;

    char *token = strtok(entrada, delimit);
    while (token != NULL) {
        ehRotulo = ehRotulo(token);
        ehDiretiva = ehDiretiva(token);
        ehInstrucao = ehInstrucao(token);
        ehHexadecimal = ehHexadecimal(token);
        ehDecimal = ehDecimal(token);
        ehInstrucao = ehNome(token);
//        if (ehRotulo) {
//            if (!achouRotulo && !achouDiretiva) {
//                achouRotulo = 1;
//            } else {
//                //retulo depois de rotulo ou diretiva
//            }
//        }
//        if (ehDiretiva) {
//            if (!achouRotulo && !achouDiretiva) {
//                achouDiretiva = 1;
//            } else {
//                //diretiva depois de rotulo ou diretiva
//            }
//        }


        token = strtok(NULL, delimit);
    }
    return 0;
}

int processarEntrada(char* entrada, unsigned tamanho)
{
    char delimit[] = "\n";

    char *token = strtok(entrada, delimit);
    while (token != NULL) {
        if (processaLinha(token)){
            return 1;
        }
        token = strtok(NULL, delimit);
    }
    /* printf("Você deve implementar esta função para a Parte 1.\n"); */
    return 0;
}