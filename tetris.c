#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// --- Definição das Estruturas de Dados e Constantes ---

// Tamanhos fixos das estruturas
#define TAMANHO_FILA 5
#define TAMANHO_PILHA 3

// Estrutura para representar uma peça do Tetris Stack
typedef struct {
    char tipo; // Tipo da peça ('I', 'O', 'T', 'L')
    int id;   // Identificador único da peça
} Peca;

// --- Variáveis Globais de Controle ---

// FILA CIRCULAR: Array, índices frente e re
Peca fila[TAMANHO_FILA];
int frente = 0; 
int re = 0;     

// PILHA: Array e índice do topo
Peca pilha[TAMANHO_PILHA];
int topo = -1; // -1 indica que a pilha está vazia

// Contador global para gerar IDs únicos para as peças
int proximoId = 0;

// --- Funções Auxiliares Comuns ---

/**
 * @brief Gera uma nova peça com tipo aleatório e um ID único.
 * @return Peca A nova peça gerada.
 */
Peca gerarPeca() {
    char tipos[] = {'I', 'O', 'T', 'L'};
    int indiceAleatorio = rand() % 4; 

    Peca novaPeca;
    novaPeca.tipo = tipos[indiceAleatorio];
    novaPeca.id = proximoId++; 

    return novaPeca;
}

/**
 * @brief Função de utilidade para trocar duas estruturas Peca.
 * @param a Ponteiro para a primeira peça.
 * @param b Ponteiro para a segunda peça.
 */
void trocarPecas(Peca *a, Peca *b) {
    Peca temp = *a;
    *a = *b;
    *b = temp;
}

// --- Funções da FILA (FIFO) ---

int tamanhoFila() {
    return (re - frente + TAMANHO_FILA) % TAMANHO_FILA;
}

int filaCheia() {
    // A fila só deve ter TAMANHO_FILA - 1 elementos para distinguir de vazia.
    return (tamanhoFila() == TAMANHO_FILA - 1); 
}

int filaVazia() {
    return frente == re;
}

/**
 * @brief Adiciona uma nova peça ao final da fila (Enqueue).
 */
void adicionarPecaFila() {
    if (filaCheia()) {
        printf("[Sistema] Fila cheia. Não é possível gerar nova peça.\n");
        return;
    }

    Peca nova = gerarPeca();
    fila[re] = nova;
    re = (re + 1) % TAMANHO_FILA;

    printf("[Sistema] Peça [%c %d] gerada e adicionada (Enqueue) ao final da fila.\n", nova.tipo, nova.id);
}

/**
 * @brief Remove a peça da frente da fila (Dequeue) e a retorna.
 * @return Peca A peça removida ou uma peça inválida se vazia.
 */
Peca removerPecaFila() {
    if (filaVazia()) {
        Peca pecaErro = {'X', -1};
        return pecaErro;
    }

    Peca pecaRemovida = fila[frente];
    frente = (frente + 1) % TAMANHO_FILA;

    return pecaRemovida;
}

// --- Funções da PILHA (LIFO) ---

int tamanhoPilha() {
    return topo + 1;
}

int pilhaCheia() {
    return topo == TAMANHO_PILHA - 1;
}

int pilhaVazia() {
    return topo == -1;
}

/**
 * @brief Insere uma peça no topo da pilha (Push).
 */
void pushPilha(Peca peca) {
    if (pilhaCheia()) {
        printf("\n🛑 ERRO: Pilha de Reserva cheia! Não é possível reservar mais.\n");
        return;
    }

    pilha[++topo] = peca;
    printf("✅ Peça [%c %d] reservada (Push) com sucesso.\n", peca.tipo, peca.id);
}

/**
 * @brief Remove e retorna a peça do topo da pilha (Pop).
 */
Peca popPilha() {
    if (pilhaVazia()) {
        Peca pecaErro = {'X', -1};
        return pecaErro;
    }

    return pilha[topo--];
}

// --- Funções de Ação do Jogo ---

/**
 * @brief Ação 1: Joga uma peça, removendo-a da frente da fila.
 */
void jogarPeca() {
    Peca pecaJogada = removerPecaFila();

    if (pecaJogada.id != -1) {
        printf("\n▶️ **1. JOGAR:** Peça [%c %d] jogada (Dequeue).\n", pecaJogada.tipo, pecaJogada.id);
        adicionarPecaFila(); // Reabastece a fila
    } else {
        printf("\n⚠️ **AVISO:** A Fila está vazia! Nenhuma peça foi jogada.\n");
    }
}

/**
 * @brief Ação 2: Move a peça da frente da fila para a pilha de reserva.
 */
void reservarPeca() {
    if (pilhaCheia()) {
        printf("\n⚠️ **AVISO:** A Pilha de Reserva está cheia. Não é possível reservar agora.\n");
        return;
    }

    Peca pecaReservar = removerPecaFila();

    if (pecaReservar.id != -1) {
        printf("\n📦 **2. RESERVAR:** Peça [%c %d] removida da fila...\n", pecaReservar.tipo, pecaReservar.id);
        pushPilha(pecaReservar);
        adicionarPecaFila(); // Reabastece a fila
    } else {
        printf("\n⚠️ **AVISO:** A Fila está vazia! Nenhuma peça para reservar.\n");
    }
}

/**
 * @brief Ação 3: Usa uma peça, removendo-a do topo da pilha.
 */
void usarPecaReservada() {
    Peca pecaUsada = popPilha();

    if (pecaUsada.id != -1) {
        printf("\n↩️ **3. USAR RESERVA:** Peça [%c %d] removida do topo da reserva (Pop) e usada.\n", 
               pecaUsada.tipo, pecaUsada.id);
    } else {
        printf("\n⚠️ **AVISO:** A Pilha de Reserva está vazia! Nenhuma peça para usar.\n");
    }
}

/**
 * @brief Ação 4: Troca a peça da frente da fila com o topo da pilha (Troca Simples).
 */
void trocarPecaAtual() {
    if (filaVazia() || pilhaVazia()) {
        printf("\n❌ **4. TROCA SIMPLES:** Requer que a Fila E a Pilha não estejam vazias para a troca.\n");
        return;
    }
    
    // Posição da frente na fila
    int indiceFila = frente; 
    
    printf("\n🔁 **4. TROCA SIMPLES:** Trocando Peça da Fila [%c %d] com Pilha [%c %d].\n",
           fila[indiceFila].tipo, fila[indiceFila].id,
           pilha[topo].tipo, pilha[topo].id);

    // Troca o valor da peça na frente da fila com o valor da peça no topo da pilha
    trocarPecas(&fila[indiceFila], &pilha[topo]);

    printf("✅ Troca simples realizada com sucesso.\n");
}

/**
 * @brief Ação 5: Alterna as três primeiras peças da fila com as três peças da pilha (Troca Múltipla).
 */
void trocarMultipla() {
    if (tamanhoFila() < 3 || tamanhoPilha() < 3) {
        printf("\n❌ **5. TROCA MÚLTIPLA:** Requer no mínimo 3 peças na Fila (%d/3) E 3 peças na Pilha (%d/3).\n",
               tamanhoFila(), tamanhoPilha());
        return;
    }

    printf("\n🔄 **5. TROCA MÚLTIPLA:** Realizando troca em bloco das 3 primeiras peças da Fila com as 3 peças da Pilha.\n");
    
    // Troca as 3 peças, iterando sobre o topo da pilha (0 a 2)
    for (int i = 0; i < 3; i++) {
        // Índice da peça na fila, calculado de forma circular a partir da 'frente'
        int indiceFila = (frente + i) % TAMANHO_FILA;
        
        // Índice da peça na pilha, calculado a partir da 'base' (topo - 2, topo - 1, topo)
        int indicePilha = topo - 2 + i;
        
        // Troca o valor
        trocarPecas(&fila[indiceFila], &pilha[indicePilha]);
    }
    
    printf("✅ Troca em bloco realizada com sucesso.\n");
}

// --- Funções de Exibição ---

/**
 * @brief Exibe o estado atual da Fila e da Pilha.
 */
void exibirEstado() {
    printf("\n==================================================================\n");
    printf("                  ESTADO ATUAL DO TETRIS STACK (MESTRE)\n");
    printf("------------------------------------------------------------------\n");
    
    // Exibição da FILA
    printf("🗄️ Fila de Peças Futuras (Frente -> Re): ");
    if (filaVazia()) {
        printf("[Vazia]");
    } else {
        int i = frente;
        while (i != re) {
            printf("[%c %d]", fila[i].tipo, fila[i].id);
            // Indica a frente da fila
            if (i == frente) printf("(F)"); 
            
            if ((i + 1) % TAMANHO_FILA != re) {
                 printf(" -> ");
            }
            i = (i + 1) % TAMANHO_FILA;
        }
    }
    printf("\n   (Elementos: %d/%d)\n", tamanhoFila(), TAMANHO_FILA - 1);
    
    // Exibição da PILHA
    printf("\n📦 Pilha de Reserva (Topo -> Base): ");
    if (pilhaVazia()) {
        printf("[Vazia]");
    } else {
        for (int i = topo; i >= 0; i--) {
            printf("[%c %d]", pilha[i].tipo, pilha[i].id);
            // Indica o topo da pilha
            if (i == topo) printf("(T)"); 
            
            if (i > 0) {
                 printf(" -> ");
            }
        }
    }
    printf("\n   (Elementos: %d/%d)\n", topo + 1, TAMANHO_PILHA);
    printf("==================================================================\n");
}

/**
 * @brief Exibe o menu de opções.
 */
void exibirMenu() {
    printf("\n--- Opções de Ação (Mestre) ---\n");
    printf("| Código | Ação                                         |\n");
    printf("|:------:|:---------------------------------------------|\n");
    printf("| 1      | Jogar peça da frente da fila (Dequeue)       |\n");
    printf("| 2      | Enviar peça da fila para a pilha de reserva  |\n");
    printf("| 3      | Usar peça da pilha de reserva                |\n");
    printf("| 4      | Trocar peça da frente da fila com o topo da pilha (Simples) |\n");
    printf("| 5      | Trocar os 3 primeiros da fila com as 3 peças da pilha (Múltipla) |\n");
    printf("| 0      | Sair                                         |\n");
    printf("-------------------------------------------------------\n");
    printf("Escolha uma ação (0-5): ");
}

/**
 * @brief Inicializa a fila preenchendo-a até a capacidade máxima.
 */
void inicializarJogo() {
    srand(time(NULL)); 
    printf("🚀 Inicializando o Gerenciamento de Peças Mestre do Tetris Stack...\n");
    
    // Inicializa a fila com 4 peças (TAMANHO_FILA - 1)
    for (int i = 0; i < TAMANHO_FILA - 1; i++) {
        adicionarPecaFila(); 
    }
    // Inicializa a pilha com 0 peças
    topo = -1;
    
    printf("\nInicialização concluída.\n");
}


// --- Função Principal ---

int main() {
    int escolha;

    // 1. Inicializa as estruturas
    inicializarJogo();
    exibirEstado();

    do {
        // 2. Exibe o menu e solicita a entrada
        exibirMenu();
        if (scanf("%d", &escolha) != 1) {
            // Limpa o buffer de entrada em caso de erro (não numérico)
            while (getchar() != '\n');
            escolha = -1; 
        }

        // 3. Processa a escolha
        switch (escolha) {
            case 1:
                jogarPeca();
                break;
            case 2:
                reservarPeca();
                break;
            case 3:
                usarPecaReservada();
                break;
            case 4:
                trocarPecaAtual();
                break;
            case 5:
                trocarMultipla();
                break;
            case 0:
                printf("\n👋 Jogo Tetris Stack encerrado. Mestre, até a próxima!\n");
                break;
            default:
                printf("\n❌ Opção inválida. Por favor, escolha um número entre 0 e 5.\n");
                break;
        }

        // 4. Exibe o estado após a ação
        if (escolha != 0) {
            exibirEstado();
        }

    } while (escolha != 0);

    return 0;
}


