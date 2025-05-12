#include "algoritmo.h"

// ----------------------------------------------------
// Funções para gerenciamento da lista de prioridade
// ----------------------------------------------------

//INICIALIZA LISTA DE PRIORIDADE
void inicializarLista(ListaPrioridade *lista) {
    lista->inicio = NULL;
    lista->tamanho = 0;
}

//INSERE UM NÓ NA LISTA PARA MANTENDO A ORDENAÇÃO POR FREQUENCIA CRESCENTE
void inserirOrdenado(No *no, ListaPrioridade *lista) {
    if (lista->inicio == NULL || no->frequencia < lista->inicio->frequencia) {
        no->proximo = lista->inicio;
        lista->inicio = no;
    } else {
        No *atual = lista->inicio;
        while (atual->proximo && atual->proximo->frequencia <= no->frequencia)
            atual = atual->proximo;
        no->proximo = atual->proximo;
        atual->proximo = no;
    }
    lista->tamanho++;
}

//REMOVE E RETORNA O PRIMEIRO NÓ (MENOR FREQUENCIA) DA LISTA
No *removerPrimeiro(ListaPrioridade *lista) {
    if (!lista->inicio) return NULL;
    No *removido = lista->inicio;
    lista->inicio = removido->proximo;
    lista->tamanho--;
    return removido;
}

//CONTA A FREQUENCIA DE CADA SÍMBOLO NO ARQUIVO
void contarFrequencias(const char *nomeArquivo, unsigned int frequencias[]) {
    FILE *arquivo = fopen(nomeArquivo, "rb");
    if (!arquivo) return;

    unsigned char buffer[TAMANHO_BUFFER];
    size_t bytesLidos;
    memset(frequencias, 0, TAMANHO_TABELA * sizeof(unsigned int));

    while ((bytesLidos = fread(buffer, 1, TAMANHO_BUFFER, arquivo)) > 0) {
        for (size_t i = 0; i < bytesLidos; i++)
            frequencias[buffer[i]]++;
    }
    fclose(arquivo);
}

// ----------------------------------------------------
// Funções para construção da árvore de Huffman
// ----------------------------------------------------

//CRIA UM NÓ FOLHA COM O SÍMBOLO E FREQUENCIA INFORMADO
No *criarFolha(unsigned char simbolo, unsigned int frequencia) {
    No *novo = calloc(1, sizeof(No));
    novo->simbolo = simbolo;
    novo->frequencia = frequencia;
    return novo;
}

//CRIA UM NÓ INTERNO UNINDO DUAS SUBARVORES
No *criarNoInterno(No *esquerdo, No *direito) {
    No *novo = malloc(sizeof(No));
    novo->frequencia = esquerdo->frequencia + direito->frequencia;
    novo->esquerda = esquerdo;
    novo->direita = direito;
    return novo;
}

//CONSTROI A ÁRVORE DE HUFFMAN A A PARTIR DE UM VETOR DE FREQUENCIA
No *construirArvoreHuffman(unsigned int frequencias[]) {
    ListaPrioridade lista;
    inicializarLista(&lista);

    for (int i = 0; i < TAMANHO_TABELA; i++) {
        if (frequencias[i] > 0)
            inserirOrdenado(criarFolha((unsigned char)i, frequencias[i]), &lista);
    }

    while (lista.tamanho > 1) {
        No *esquerdo = removerPrimeiro(&lista);
        No *direito = removerPrimeiro(&lista);
        inserirOrdenado(criarNoInterno(esquerdo, direito), &lista);
    }
    return lista.inicio;
}

//PERCORRE A ÁRVORE E GERA UM CÓDIGO BINÁRIO PARA CADA SÍMBOLO
void gerarCodigos(No *no, char caminho[], int posicao, char tabelaCodigos[TAMANHO_TABELA][TAMANHO_TABELA]) {
    if (!no->esquerda && !no->direita) {
        caminho[posicao] = '\0';
        strcpy(tabelaCodigos[no->simbolo], caminho);
        return;
    }
    if (no->esquerda) {
        caminho[posicao] = '0';
        gerarCodigos(no->esquerda, caminho, posicao + 1, tabelaCodigos);
    }
    if (no->direita) {
        caminho[posicao] = '1';
        gerarCodigos(no->direita, caminho, posicao + 1, tabelaCodigos);
    }
}

// ----------------------------------------------------
// Funções para escrita de bits no arquivo
// ----------------------------------------------------

//INICIALIZA O CONTROLADOR DE BITS PARA ESCRITA EM ARQUIVOS
void inicializarControlador(ControladorBits *controlador, FILE *arquivo) {
    controlador->arquivo = arquivo;
    controlador->buffer = 0;
    controlador->bitsEscritos = 0;
    controlador->totalBits = 0;
}

//ESCREVE UM ÚNICO BIT NO ARQUIVO USANDO BUFFER DE 8 BITS
void escreverBit(ControladorBits *controlador, int bit) {
    controlador->buffer = (controlador->buffer << 1) | (bit & 1);
    controlador->bitsEscritos++;
    controlador->totalBits++;
    if (controlador->bitsEscritos == 8) {
        fputc(controlador->buffer, controlador->arquivo);
        controlador->buffer = 0;
        controlador->bitsEscritos = 0;
    }
}

//FINALIZA A ESCRITA PREENCHENDO O BUFFER COM ZERO SE NECESSÁRIO
void finalizarEscrita(ControladorBits *controlador) {
    if (controlador->bitsEscritos > 0) {
        controlador->buffer <<= (8 - controlador->bitsEscritos);
        fputc(controlador->buffer, controlador->arquivo);
    }
}

//SERIALIZA A ÁRVORE DE HUFFMAN EM ARQUIVO USANDO PERCURSO PRÉ-ORDEM
int escreverArvore(No *no, FILE *arquivo) {
    if (!no->esquerda && !no->direita) {
        if (no->simbolo == '*' || no->simbolo == '\\') {
            fputc('\\', arquivo);
            fputc(no->simbolo, arquivo);
            return 2;
        } else {
            fputc(no->simbolo, arquivo);
            return 1;
        }
    }
    fputc('*', arquivo);
    int tamEsquerdo = escreverArvore(no->esquerda, arquivo);
    int tamDireito = escreverArvore(no->direita, arquivo);
    return 1 + tamEsquerdo + tamDireito;
}

// ----------------------------------------------------
// Funções principais de compactação e descompactação
// ----------------------------------------------------

//REALIZA A COMPACTAÇÃO DO ARQUIVO UTILIZANDO HUFFMAN
void compactarHuffman(const char *nomeEntrada, const char *nomeSaida) {
    unsigned int frequencias[TAMANHO_TABELA] = {0};
    contarFrequencias(nomeEntrada, frequencias);

    No *raiz = construirArvoreHuffman(frequencias);

    char tabelaCodigos[TAMANHO_TABELA][TAMANHO_TABELA] = {{0}};
    char caminhoAtual[TAMANHO_TABELA];
    gerarCodigos(raiz, caminhoAtual, 0, tabelaCodigos);

    //Abre o arquivo de saída e guarda dois bytes pro cabeçalho
    FILE *saida = fopen(nomeSaida, "wb");
    if (!saida) return;

    fputc(0, saida);
    fputc(0, saida);

    //A arvore é serializada em pré-ordem e escrita no inicio do arquivo
    int tamanhoArvore = escreverArvore(raiz, saida);

    FILE *entrada = fopen(nomeEntrada, "rb");
    if (!entrada) {
        fclose(saida);
        return;
    }

    //inicializa o controlador de bits
    ControladorBits controlador;
    inicializarControlador(&controlador, saida);

    //Lê o arquivo de entrada e escreve os bits correspondentes
    int caractere;
    while ((caractere = fgetc(entrada)) != EOF) {
        //Para cada caractere lido, acessa seu código binário na tabela de códigos
        char *codigo = tabelaCodigos[caractere];
        //insere cada bit no buffer até completar 8bits 
        for (int i = 0; codigo[i] != '\0'; i++)
            //converte cada dígito em seu valor numérico
            escreverBit(&controlador, codigo[i] - '0');
    }
    //se o ultimo byte for incompleto, preenche com 0s e grava no arquivo
    finalizarEscrita(&controlador);
    //calcula quantos bits são de lixo no ultimo byte
    int bitsLixo = (8 - (controlador.totalBits % 8)) % 8;
    //cria o cabeçalho com o tamanho da arvore e bits lixo
    //garante que pro tamanho da arvore, sejam pegos os 13 menos significativos
    unsigned short cabecalho = (bitsLixo << 13) | (tamanhoArvore & 0x1FFF);

    //vai pro inicio do arquivo de saída
    fseek(saida, 0, SEEK_SET);
    //escreve o primeiro byte do cabeçalho com a parte mais significativa
    fputc((cabecalho >> 8) & 0xFF, saida);
    //escreve o segundo byte do cabeçalho com a parte menos significativa
    fputc(cabecalho & 0xFF, saida);

    fclose(entrada);
    fclose(saida);
}

//LÊ O CABEÇALHO DE UM ARQUIVO COMPACTADO, EXTRAINDO INFORMAÇÕES DO CONTROLE
void lerCabecalho(FILE *arquivo, int *bitsLixo, int *tamanhoArvore) {
    //volta pro inicio do arquivo
    fseek(arquivo, 0, SEEK_SET);
    //lê os dois primeiros bytes
    int byte1 = fgetc(arquivo);
    int byte2 = fgetc(arquivo);
    //pega os 3 primeiros bits do byte1 que são o lixo
    *bitsLixo = byte1 >> 5;
    //pega os 5 bits restantes para formar os 13 mias significativos, combinando com byte2
    *tamanhoArvore = (byte1 & 0x1F) << 8 | byte2;
}

//RECONSTROI A ÁRVORE A PARTIR DE ARQUIVOS SEREALIZADOS
No *lerNo(FILE *arquivo) {
    int caractere = fgetc(arquivo);
    if (caractere == EOF) return NULL;

    if (caractere == '\\') {
        return criarFolha(fgetc(arquivo), 0);
    } else if (caractere == '*') {
        No *no = malloc(sizeof(No));
        no->esquerda = lerNo(arquivo);
        no->direita = lerNo(arquivo);
        return no;
    } else {
        return criarFolha(caractere, 0);
    }
}

//DECODIFICA DADOS COMPACTADOS EM BITS, ATRAVESSANDO A ÁRVORE DE HUFFMAN
void decodificarBits(FILE *entrada, FILE *saida, No *raiz, int bitsLixo) {
    No *atual = raiz;
    uint8_t byteAtual, proximoByte;
    int finalizado = 0;

    //tenta ler 1 byte do arquivo de entrada  pra armazenar na variavel byteatual
    //se o arquivo acabar, a função retorna
    size_t leitura = fread(&byteAtual, 1, 1, entrada);
    if (leitura != 1) return;

    while (!finalizado) {
        //tenta ler o proximo byte do arquivo
        size_t leituraProx = fread(&proximoByte, 1, 1, entrada);
        //caso existam bits restantes, é o byte final do arquivo
        //caso seja, desconsidera os bits lixo
        int bitsRestantes = (leituraProx == 1) ? 8 : 8 - bitsLixo;
        //finaliza o loop se não tive próx a ser lido
        finalizado = (leituraProx != 1);

        //lê cada bit de byteAtual, do mais significativo pro menos significativo
        for (int i = 7; i >= 8 - bitsRestantes; i--) {
            int bit = (byteAtual >> i) & 1;
            //usa esse bit pra navegar pela arvore (0->esquerda, 1->direita)
            atual = (bit == 0) ? atual->esquerda : atual->direita;

            //quando chega numa folha, escreve o simbolo no arquivo de saída
            if (!atual->esquerda && !atual->direita) {
                fputc(atual->simbolo, saida);
                atual = raiz;
            }
        }
        //atualiza o byte atual e continua o loop
        byteAtual = proximoByte;
    }
}

//REALIZAM A DESCOMPACTAÇÃO DE UM ARQUIVO
void descompactarHuffman(const char *nomeEntrada, const char *nomeSaida) {
    FILE *entrada = fopen(nomeEntrada, "rb");
    if (!entrada) return;

    FILE *saida = fopen(nomeSaida, "wb");
    if (!saida) {
        fclose(entrada);
        return;
    }

    int bitsLixo, tamanhoArvore;
    lerCabecalho(entrada, &bitsLixo, &tamanhoArvore);

    No *raiz = lerNo(entrada);
    if (!raiz) {
        fclose(entrada);
        fclose(saida);
        return;
    }

    decodificarBits(entrada, saida, raiz, bitsLixo);

    fclose(entrada);
    fclose(saida);
}

//GERA AUTOMATICAMENTE O NOME DE SAIDA .HUFF
void gerarNomeArquivoComExtensaoHuff(char *entrada, char *saida) {
    const char *ext = strrchr(entrada, '.');
    size_t len = ext ? (size_t)(ext - entrada) : strlen(entrada);
    memcpy(saida, entrada, len);
    strcpy(saida + len, ".huff");
}



