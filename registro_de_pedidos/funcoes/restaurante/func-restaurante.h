// Arquivo que contém os protópipos das funções relacionadas à interação do usuário-restaurante com o programa.

#pragma once

#include <stdbool.h>

#define CARDAPIO_DB "cardapio.bin" // Arquivo do cardápio
#define TEMP_DB "cardapio.tmp"     // Arquivo temporário para operações seguras

typedef enum {COMIDA, BEBIDA, SOBREMESA} TipoItem; // Enumeração para tipos de itens do cardápio

// Estrutura que representa um item do cardápio
typedef struct {
    char nome[50];
    TipoItem tipo;
    float valor;
    bool ativo; // Indica se o item está ativo (disponível) ou não. Importante para deleção lógica
} ItemCardapio;

// Gerenciamento do Cardápio
bool adicionar_item();
bool editar_item();
bool excluir_item();
void exibir_cardapio();

// Funções auxiliares
ItemCardapio* carregar_cardapio(int* total_itens); // Carrega o cardápio do arquivo binário
bool salvar_cardapio(ItemCardapio* itens, int total); // Salva o cardápio no arquivo binário
int contar_itens_ativos(); // Conta itens ativos no cardápio
void liberar_cardapio(ItemCardapio* itens); // Libera memória alocada para o cardápio
static bool verificar_permissoes(); // Verifica se o usuário tem permissão para modificar o cardápio
static bool arquivo_existe(const char* filename); // Verifica se o arquivo existe

// Função que exibe o menu do restaurante
// Ela é responsável por apresentar as opções disponíveis para o restaurante e processar a escolha do usuário
void exibir_menu_restaurante();