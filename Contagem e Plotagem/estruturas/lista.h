
/*
    #ifndef -> se LISTA_H ainda não foi definido 
    "include guard" (ou "guarda de inclusão").
    evita muitos bugs chatos, especialmente quando você trabalha com múltiplos arquivos
    evitar que o mesmo arquivo .h seja incluído mais de uma vez no mesmo programa
    o que pode causar erros de redefinição de structs, funções ou variávies.
    #endif -> fecha o #ifndef
*/

#ifndef LISTA_H
#define LISTA_H

typedef struct NoLista {
    int valor;
    struct NoLista* prox;
} NoLista;

void inserir_lista(NoLista** head, int valor);
int buscar_lista(NoLista* head, int chave, int* comparacoes);
void liberar_lista(NoLista* head);

#endif

