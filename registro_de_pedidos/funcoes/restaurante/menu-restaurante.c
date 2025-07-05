// Arquivo com a implementação das funções relacionadas à interação do usuário-restaurante com o programa. 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Definições de constantes e estruturas
#include "func-restaurante.h"
#include "../auxiliares.h"
#include "../cliente/func-cliente.h"

// Exibe o menu do restaurante, permitindo ao usuário interagir com o cardápio
// e realizar operações como visualizar, adicionar, editar ou remover itens.
void exibir_menu_restaurante() {
    int opcao;
    top_bottom();
    printf("BEM VINDO(A) AO MENU DO RESTAURANTE\n");
    mid_line();

    do {
        printf("ESCOLHA UMA OPCAO ABAIXO:\n");
        mid_line();
        printf("[1] VISUALIZAR CARDAPIO\n");
        printf("[2] ADICIONAR ITEM AO CARDAPIO\n");
        printf("[3] EDITAR ITEM DO CARDAPIO\n");
        printf("[4] REMOVER ITEM DO CARDAPIO\n");
        printf("[5] GERAR RELATORIO FINAL\n");
        printf("[6] VOLTAR AO MENU ANTERIOR\n");
        mid_line();

        opcao = ler_inteiro_seguro("Digite a opcao desejada:", 1, 6); // Entrada segura para a opção

        switch(opcao) {
            case 1:
                exibir_cardapio(); // Exibe o cardápio atual
                break;

            case 2:
                if (!adicionar_item()){ // Tenta adicionar um novo item ao cardápio
                    printf("\033[1;31mFalha ao adicionar item!\033[0m\n"); // Mensagem de erro se falhar ao adicionar
                    mid_line();
                }
                break;

            case 3:
                if(!editar_item()) { // Tenta editar um item existente no cardápio
                    printf("\033[1;31mFalha ao editar item!\033[0m\n"); // Mensagem de erro se falhar ao editar
                    mid_line();
                }
                break;
            
            case 4:
                if(!excluir_item()) { // Tenta remover um item do cardápio
                    printf("\033[1;31mFalha ao remover item!\033[0m\n"); // Mensagem de erro se falhar ao remover
                    mid_line();
                }
                break;

            case 5:
                system("clear || cls"); // Limpa a tela para melhor visualização
                gerar_relatorio(); // Gera um relatório final dos pedidos
                break;

            case 6:
                system("clear || cls"); // Limpa a tela para melhor visualização
                printf("VOLTANDO AO MENU ANTERIOR...\n");

                top_bottom(); 
                printf("BEM VINDO(A) AO SISTEMA DE PEDIDOS DO RESTAURANTE\n");
                mid_line();
                break;

            default: // Caso a opção seja inválida, informa o usuário
                printf("\033[1;31mOpcao invalida! Tente novamente.\033[0m\n");
                break;
        }
    } while (opcao != 6); // Loop infinito para manter o menu ativo até que o usuário escolha sair
}

