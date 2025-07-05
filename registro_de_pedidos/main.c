// Arquivo principal do projeto, que simula o menu inicial de um restaurante

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "funcoes/auxiliares.h"
#include "funcoes/cliente/func-cliente.h"
#include "funcoes/restaurante/func-restaurante.h"

#define SENHA "adminPass" // Definindo a senha de administrador
#define MAX_TENTATIVAS 3  // Número máximo de tentativas para inserir a senha

int main(){
    int opcao, tentativas = 0;
    char senha[20];

    system("clear || cls"); // Limpa a tela para melhor visualização

    top_bottom();

    printf("BEM VINDO(A) AO SISTEMA DE PEDIDOS DO RESTAURANTE\n");

    mid_line();

    do {
        printf("ESCOLHA UMA OPCAO ABAIXO:\n");
        mid_line();
        printf("[1] MENU DO CLIENTE\n");
        printf("[2] MENU DO RESTAURANTE\n");
        printf("[3] SAIR\n");

        mid_line();
        opcao = ler_inteiro_seguro("Digite a opcao desejada:", 1, 3); // Entrada segura para a opção

        switch(opcao){
            case 1:
                system("clear || cls"); // Limpa a tela para melhor visualização
                exibir_menu_cliente(); // Chama a função que exibe o menu do cliente
                //while(getchar() != '\n'); // Limpa o buffer de entrada
                break;

            case 2:
                system("clear || cls"); // Limpa a tela para melhor visualização
                
                top_bottom();
                
                printf("ESTA E UMA AREA RESTRITA!\n"); // Mensagem de aviso para área restrita
                printf("Digite a senha de administrador:\n");

                fgets(senha, sizeof(senha), stdin);
                senha[strcspn(senha, "\n")] = '\0';

                if (strcmp(senha, SENHA) == 0) {

                    tentativas = 0; // Reseta o contador de tentativas

                    printf("\033[1;32mAcesso concedido ao menu do restaurante.\033[0m\n");
                    exibir_menu_restaurante(); // Exibe o menu do restaurante

                } else {

                    // Se a senha estiver incorreta, inicia o loop de tentativas

                    while (tentativas < MAX_TENTATIVAS && strcmp(senha, SENHA) != 0) {
                        tentativas++;
                        if (tentativas < MAX_TENTATIVAS) {

                            mid_line();

                            // Mensagem de erro para senha incorreta

                            printf("\033[1;31mSenha incorreta! Tente novamente:\033[0m\n");
                            fgets(senha, sizeof(senha), stdin);
                            senha[strcspn(senha, "\n")] = '\0';
                        }
                    }
                    if (strcmp(senha, SENHA) != 0) {

                        mid_line();

                        // Mensagem de erro para número máximo de tentativas excedido
                        
                        printf("\033[1;31mNumero maximo de tentativas excedido. Acesso negado.\033[0m\n");
                    } else {

                        mid_line();
                        tentativas = 0; // Reseta o contador de tentativas

                        printf("\033[1;32mAcesso concedido ao menu do restaurante.\033[0m\n");
                        exibir_menu_restaurante(); // Exibe o menu do restaurante
                    }
                }
                break;

            case 3:
                mid_line();
                printf("ENCERRANDO SISTEMA...OBRIGADO E VOLTE SEMPRE!\n");
                top_bottom();
                
                exit(0);
                break;

            default:
                printf("\033[1;31mOpcao invalida! Tente novamente.\033[0m\n");
                break;
        }
    } while (1); // Loop infinito para manter o menu ativo até que o usuário escolha sair
    
    return 0;
}