#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "estruturas/lista.h"
#include "estruturas/arvore.h"

// Constantes de configuração
#define NUMERO_DE_BUSCAS 1000       // Quantidade de buscas a serem realizadas em cada tamanho de estrutura
#define VALOR_MAXIMO 1000           // Valor máximo para os números aleatórios gerados

// Função de comparação para qsort
// Recebe dois ponteiros genéricos (void*), os converte para int*, e retorna a diferença entre eles. 
// Isso determina a ordem crescente.
int comparar_inteiros(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

// Função para inserção balanceada na árvore (cria uma árvore balanceada a partir de um array ordenado)
void inserir_balanceado(NoArvore** raiz_arvore, int* valores_ordenados, int inicio, int fim) {
    // Caso base: quando o intervalo é inválido
    if (inicio > fim) return;
    
    // Encontra o meio do intervalo atual
    int meio = inicio + (fim - inicio) / 2;
    
    // Insere o valor do meio na árvore
    inserir_arvore(raiz_arvore, valores_ordenados[meio]);
    
    // Insere recursivamente os valores à esquerda e à direita do meio
    inserir_balanceado(raiz_arvore, valores_ordenados, inicio, meio - 1);
    inserir_balanceado(raiz_arvore, valores_ordenados, meio + 1, fim);
}

int main() {
    // Tamanhos das estruturas de dados que serão testados
    int tamanhos_estruturas[] = {1000, 5000, 10000, 50000, 100000};
    // Calcula quantos tamanhos diferentes foram definidos (nesse caso, 5)
    int quantidade_tamanhos = sizeof(tamanhos_estruturas) / sizeof(tamanhos_estruturas[0]);

    // Abre arquivo CSV para gravar os resultados
    FILE* arquivo_saida = fopen("output/comparacoes.csv", "w"); //"w" indica que o arquivo será sobrescrito a cada execução.
    if (arquivo_saida == NULL) {
        printf("Erro ao abrir arquivo de saída!\n");
        return 1;
    }
    // Escreve cabeçalho do CSV
    fprintf(arquivo_saida, "tamanho,media_lista,media_arvore\n");

    // Inicializa semente para números aleatórios
    // Garante que cada execução do programa produza resultados diferentes
    srand(time(NULL));

    // Loop principal: testa cada tamanho de estrutura
    for (int i = 0; i < quantidade_tamanhos; i++) {
        int tamanho_atual = tamanhos_estruturas[i];
        
        // Aloca memória para os valores que serão inseridos nas estruturas
        int* valores_aleatorios = malloc(tamanho_atual * sizeof(int));
        if (!valores_aleatorios) {
            printf("Erro ao alocar memória para tamanho = %d\n", tamanho_atual);
            fclose(arquivo_saida);
            return 1;
        }

        // Inicializa estruturas
        NoLista* lista_encadeada = NULL;
        NoArvore* arvore_binaria = NULL;

        // Preenche a lista encadeada com valores aleatórios
        for (int j = 0; j < tamanho_atual; j++) {
            valores_aleatorios[j] = rand() % VALOR_MAXIMO;
            inserir_lista(&lista_encadeada, valores_aleatorios[j]);
        }

        // Ordena os valores e insere balanceado na árvore
        qsort(valores_aleatorios, tamanho_atual, sizeof(int), comparar_inteiros);
        inserir_balanceado(&arvore_binaria, valores_aleatorios, 0, tamanho_atual - 1);

        // Variáveis para acumular o total de comparações
        int total_comparacoes_lista = 0;
        int total_comparacoes_arvore = 0;

        // Realiza as buscas nas estruturas
        for (int busca = 0; busca < NUMERO_DE_BUSCAS; busca++) {
            int valor_procurado;
            
            // 20% das buscas serão por valores não existentes (para teste)
            if (busca % 5 == 0) {
                valor_procurado = VALOR_MAXIMO + rand() % VALOR_MAXIMO;
            } else {
                // Busca por valor existente (seleciona aleatoriamente um índice)
                int indice_aleatorio = rand() % tamanho_atual;
                valor_procurado = valores_aleatorios[indice_aleatorio];
            }

            // Variáveis para contar comparações em cada busca
            int comparacoes_lista = 0, comparacoes_arvore = 0;
            
            // Executa as buscas e conta as comparações
            buscar_lista(lista_encadeada, valor_procurado, &comparacoes_lista);
            buscar_arvore(arvore_binaria, valor_procurado, &comparacoes_arvore);

            // Acumula o total de comparações
            total_comparacoes_lista += comparacoes_lista;
            total_comparacoes_arvore += comparacoes_arvore;
        }

        // Calcula médias de comparações
        double media_comparacoes_lista = (double)total_comparacoes_lista / NUMERO_DE_BUSCAS;
        double media_comparacoes_arvore = (double)total_comparacoes_arvore / NUMERO_DE_BUSCAS;

        // Exibe resultados no console
        printf("Tamanho: %6d | Lista (O(n)): %8.2f | Árvore (O(log n)): %6.2f\n", 
               tamanho_atual, media_comparacoes_lista, media_comparacoes_arvore);
        
        // Grava resultados no arquivo CSV
        fprintf(arquivo_saida, "%d,%.2f,%.2f\n", tamanho_atual, media_comparacoes_lista, media_comparacoes_arvore);

        // Libera memória das estruturas e array
        liberar_lista(lista_encadeada);
        liberar_arvore(arvore_binaria);
        free(valores_aleatorios);
    }

    // Fecha arquivo e finaliza
    fclose(arquivo_saida);
    printf("Dados salvos em comparacoes.csv\n");
    return 0;
}