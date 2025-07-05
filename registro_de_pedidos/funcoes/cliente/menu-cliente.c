// Arquivo que com a implementação das funções relacionadas à interação do usuário-cliente com o programa.

#include <stdio.h>
#include <stdlib.h>

#include "func-cliente.h"
#include "../restaurante/func-restaurante.h"
#include "../auxiliares.h"


// Função que exibe o menu do cliente
// Ela é responsável por apresentar as opções disponíveis para o cliente e processar a escolha do usuário
void exibir_menu_cliente(){
    int opcao;
    top_bottom();
    printf("BEM VINDO(A) AO MENU DO CLIENTE\n");
    mid_line();

    do {
        printf("ESCOLHA UMA OPCAO ABAIXO:\n");
        mid_line();
        printf("[1] VISUALIZAR CARDAPIO\n");
        printf("[2] REALIZAR PEDIDO\n");
        printf("[3] VOLTAR AO MENU ANTERIOR\n");
        mid_line();

        opcao = ler_inteiro_seguro("Digite a opcao desejada: ", 1, 3); // Entrada segura para a opção

        switch(opcao){
            case 1:
                system("clear || cls"); // Limpa a tela para melhor visualização
                exibir_cardapio(); // Chama a função para exibir o cardápio
                break;
            
            case 2:
                system("clear || cls"); // Limpa a tela para melhor visualização
                realizar_pedido(); // Chama a função para realizar o pedido
                break;

            case 3:
                system("clear || cls"); // Limpa a tela para melhor visualização
                printf("VOLTANDO AO MENU ANTERIOR...\n");

                top_bottom();
                printf("BEM VINDO(A) AO SISTEMA DE PEDIDOS DO RESTAURANTE\n");
                mid_line();
                break; 
            
            default:
                printf("\033[1;31mOpcao invalida! Tente novamente.\033[0m\n");
                break;
        }
    } while (opcao != 3); // Loop infinito para manter o menu ativo até que o usuário escolha sair
}
