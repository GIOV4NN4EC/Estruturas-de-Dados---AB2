#include <stdlib.h>
#include "arvore.h"

void inserir_arvore(NoArvore** raiz, int valor) {
    if (*raiz == NULL) {
        NoArvore* novo = (NoArvore*)malloc(sizeof(NoArvore));
        novo->valor = valor;
        novo->esq = novo->dir = NULL;
        *raiz = novo;
    } else {
        if (valor < (*raiz)->valor) {
            inserir_arvore(&((*raiz)->esq), valor);
        } else {
            inserir_arvore(&((*raiz)->dir), valor);
        }
    }
}

int buscar_arvore(NoArvore* raiz, int chave, int* comparacoes) {
    while (raiz != NULL) {
        (*comparacoes)++;
        if (chave == raiz->valor)
            return 1;
        if (chave < raiz->valor)
            raiz = raiz->esq;
        else
            raiz = raiz->dir;
    }
    return 0;
}

void liberar_arvore(NoArvore* raiz) {
    if (raiz != NULL) {
        liberar_arvore(raiz->esq);
        liberar_arvore(raiz->dir);
        free(raiz);
    }
}