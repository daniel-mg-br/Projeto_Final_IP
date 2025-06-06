#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include "funcoes.h"
#include "func_restaurante.h"

int main() {
    struct dados_pedido *pedidos = NULL;
    int total_pedidos = 0;
    int capacidade = 0;
    
    // Carrega cardápio
    Cardapio *cardapio = NULL;
    if (carregar_cardapio("cardapio.bin", &cardapio)) {
        printf("Erro ao carregar cardapio!\n");
        return 1;
    }

    // Verificação do cardápio
    int qtd_itens_cardapio = total_itens_cardapio("cardapio.bin");
    if (qtd_itens_cardapio == 0) {
        printf("Erro: Cardapio vazio ou não encontrado!\n");
        free(cardapio);
        return 1;
    }

    struct cupom_desconto cupons[MAX_CUPONS];
    inicializar_cupons(cupons);

    // Carrega pedidos existentes
    if (!carregar_pedidos(&pedidos, &total_pedidos, &capacidade)) {
        top_bottom();
        printf("Nenhum pedido anterior carregado. Iniciando novo dia.\n");
        mid_line();
    }

    int continuar_programa = 1;
    top_bottom();

    do {
        system("cls || clear");
        
        // Mostra cardápio completo
        if (imprimir_cardapio("cardapio.bin")) {
            printf("Erro ao carregar cardapio para visualizacao!\n");
            continue;
        }
        mid_line();

        int mesa = ler_inteiro_seguro("Numero da mesa (1-50): ", 1, 50);
        int pessoas = ler_inteiro_seguro("Quantidade de pessoas na mesa (1-20): ", 1, 20);

        int inicio_pedidos_mesa = total_pedidos;
        int continuar_pedido = 1;

        do {
            system("cls || clear");
            imprimir_cardapio("cardapio.bin");
            mid_line();

            int posicao_item = ler_inteiro_seguro("Numero do item no cardapio: ", 1, qtd_itens_cardapio);
            
            if (posicao_item < 1 || posicao_item > qtd_itens_cardapio) {
                printf("\033[1;31mItem invalido! Tente novamente.\033[0m\n");
                continue;
            }

            int qtd = ler_inteiro_seguro("Quantidade: ", 1, 1000);

            registrar_e_salvar(&pedidos, &total_pedidos, &capacidade,
                   mesa, pessoas, posicao_item,
                   cardapio, qtd_itens_cardapio, qtd, "", 0.0);

            continuar_pedido = validar_resposta_sn("Deseja pedir mais alguma coisa? ");
        } while (continuar_pedido);

        system("cls || clear");

        // Exibe resumo da mesa
        top_bottom();
        printf("RESUMO DOS PEDIDOS DA MESA %d:\n", mesa);
        mid_line();

        float total_mesa = 0;
        for (int i = inicio_pedidos_mesa; i < total_pedidos; i++) {
            exibir_pedido(pedidos[i], 0);
            total_mesa += pedidos[i].subtotal;
        }

        printf("TOTAL DA MESA (sem desconto): R$%.2f\n", total_mesa);
        top_bottom();

        // Processa cupom de desconto
        char codigo_cupom[20] = "";
        float desconto = 0.0;

        if (validar_resposta_sn("Deseja aplicar cupom de desconto?")) {
            while (1) {
                printf("Digite o codigo do cupom (ou 'n' para cancelar): ");
                fgets(codigo_cupom, sizeof(codigo_cupom), stdin);
                codigo_cupom[strcspn(codigo_cupom, "\n")] = '\0';

                if (strlen(codigo_cupom) == 1 && tolower(codigo_cupom[0]) == 'n') {
                    printf("\033[1;33mOperacao cancelada.\033[0m\n");
                    break;
                }

                if (aplicar_cupom(codigo_cupom, cupons, &desconto)) {
                    printf("\033[1;32mDesconto de %.0f%% aplicado!\033[0m\n", desconto * 100);
                    
                    // Aplica desconto aos pedidos da mesa
                    for (int i = inicio_pedidos_mesa; i < total_pedidos; i++) {
                        if (pedidos[i].mesa == mesa) {
                            strcpy(pedidos[i].cupom, codigo_cupom);
                            pedidos[i].desconto = desconto;
                        }
                    }
                    
                    // Reescreve arquivo com atualizações
                    FILE *arquivo = fopen(ARQUIVO_BINARIO, "wb");
                    if (arquivo) {
                        fwrite(pedidos, sizeof(struct dados_pedido), total_pedidos, arquivo);
                        fclose(arquivo);
                    }
                    break;
                } else {
                    printf("\033[1;31mCupom invalido! Tente novamente.\033[0m\n");
                }
            }
        }

        // Exibe total final
        float total_final = total_mesa * (1 - desconto);
        mid_line();
        printf("TOTAL FINAL DA MESA %d: R$%.2f\n", mesa, total_final);
        printf("VALOR POR PESSOA: R$%.2f\n", total_final / pessoas);
        top_bottom();

        continuar_programa = validar_resposta_sn("Deseja registrar pedidos de outra mesa?");
    } while (continuar_programa);

    // Relatório final
    if (validar_resposta_sn("Deseja gerar o relatorio final do dia?")) {
        system("cls || clear");
        gerar_relatorio_final();
    }

    // Libera memória
    free(pedidos);
    free(cardapio);
    
    printf("\nSistema encerrado. Volte sempre!\n");
    return 0;
}