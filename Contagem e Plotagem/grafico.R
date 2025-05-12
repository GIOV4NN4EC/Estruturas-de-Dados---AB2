# Lê o arquivo CSV contendo os dados de comparações
# O arquivo deve conter colunas como: tamanho, media_lista, media_arvore
dados <- read.csv("output/comparacoes.csv")

# Cria um arquivo de imagem PNG com largura de 1200px e altura de 600px
# O gráfico gerado será salvo como "grafico.png"
png("output/grafico.png", width = 1200, height = 600)

# Configura o layout gráfico para mostrar dois gráficos lado a lado (1 linha, 2 colunas)
par(mfrow = c(1, 2))

# === GRÁFICO 1: Lista Encadeada ===

# Plota o gráfico da média de comparações na lista encadeada
# 'type = "o"' desenha pontos conectados por linhas
# 'col = "red"' define a cor vermelha para os pontos e linhas
# 'pch = 19' define o símbolo do ponto como um círculo cheio
plot(dados$tamanho, dados$media_lista,
     type = "o", col = "red", pch = 19,
     xlab = "Tamanho da estrutura (n)",  # Rótulo do eixo X
     ylab = "Número médio de comparações",  # Rótulo do eixo Y
     main = "Lista")  # Título do gráfico

# === GRÁFICO 2: Árvore de Busca Binária ===

# Plota o gráfico da média de comparações na árvore binária de busca
# Utiliza o mesmo tipo de gráfico (pontos e linhas), mas agora em azul
# 'log = "y"' aplica escala logarítmica no eixo Y para evidenciar diferenças
plot(dados$tamanho, dados$media_arvore,
     type = "o", col = "blue", pch = 19,
     xlab = "Tamanho da estrutura (n)",
     ylab = "Número médio de comparações",
     main = "Árvore de busca binária",
     log = "y")  # Escala logarítmica para o eixo Y

# Finaliza o dispositivo gráfico e salva o gráfico no arquivo PNG
dev.off()
