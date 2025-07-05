// Este arquivo contém os protótipos das funções enolvendo a interação do usuário-cliente com o programa.

#pragma once
#include <stdbool.h>
#include "../restaurante/func-restaurante.h"

#define PEDIDO_DB "pedidos.bin"
#define MAX_ITENS 1000
#define MAX_MESAS 100
#define MAX_CUPONS 5

typedef struct {
    int mesa;
    int pessoas;
    char nome_item[50];
    TipoItem tipo;          // 0: Comida | 1: Bebida | 2: Sobremesa
    int quantidade;
    float subtotal;
    char cupom[20];         // Nome do cupom aplicado (ou vazio)
    float desconto_aplicado; // 0.0 se nenhum, 0.1 para 10%, 0.4 para 40% etc.
} Pedido;

typedef struct {
    char nome[50];
    int quantidade;
    float total;
} ItemEstat;

typedef struct {
    char codigo[20];
    float desconto; // Ex: 0.1 = 10%
} Cupom;

// Função que exibe o menu do cliente
// Ela é responsável por apresentar as opções disponíveis para o cliente e processar a escolha do usuário
void exibir_menu_cliente();

// Função que inicializa os cupons disponíveis
// Ela preenche um array de cupons com os cupons disponíveis e o total de cupons
void inicializar_cupons(Cupom* cupons, int* total);

// Função responsável por verificar se o cupom é válido
// Ela recebe a entrada do usuário, verifica se o cupom existe no array de cupons
int validar_cupom(const char* entrada, Cupom* cupons, int total, float* desconto);

// Função responsável por realizar o pedido do cliente
// Ela coleta os dados do pedido, exibe o cardápio, calcula o total e aplica o desconto se necessário
void realizar_pedido();

// Função que calcula métricas e gera um relatório dos pedidos
// Ela lê os pedidos do arquivo, calcula o total arrecadado, média por mesa, e outras estatísticas
void gerar_relatorio();

