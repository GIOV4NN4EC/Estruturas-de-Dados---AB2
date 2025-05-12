#include <stdlib.h>
#include "lista.h"

void inserir_lista(NoLista** head, int valor) {
    NoLista* novo = (NoLista*)malloc(sizeof(NoLista));
    novo->valor = valor;
    novo->prox = *head;
    *head = novo;
}

int buscar_lista(NoLista* head, int chave, int* comparacoes) {
    while (head != NULL) {
        (*comparacoes)++;
        if (head->valor == chave)
            return 1;
        head = head->prox;
    }
    return 0;
}

void liberar_lista(NoLista* head) {
    while (head != NULL) {
        NoLista* temp = head;
        head = head->prox;
        free(temp);
    }
}