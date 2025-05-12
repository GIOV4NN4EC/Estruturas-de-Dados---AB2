#ifndef ARVORE_H
#define ARVORE_H

typedef struct NoArvore {
    int valor;
    struct NoArvore* esq;
    struct NoArvore* dir;
} NoArvore;

void inserir_arvore(NoArvore** raiz, int valor);
int buscar_arvore(NoArvore* raiz, int chave, int* comparacoes);
void liberar_arvore(NoArvore* raiz);

#endif