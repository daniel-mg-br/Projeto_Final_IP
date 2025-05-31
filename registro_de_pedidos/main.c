#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "funcoes.h"

int main() {
    struct dados_pedido *pedidos = NULL;
    int total_pedidos = 0;
    int capacidade = 0;

    struct cupom_desconto cupons[MAX_CUPONS];
    inicializar_cupons(cupons);

    // Carrega os pedidos existentes do arquivo binário para o vetor dinamicamente alocado
    if (!carregar_pedidos(&pedidos, &total_pedidos, &capacidade)) {
        printf("Nenhum pedido anterior carregado ou erro na leitura.\n");
    }

    int continuar_programa = 1;

    do {
        int mesa = ler_inteiro_seguro("Numero da mesa (1-50): ", 1, 50);
        int pessoas = ler_inteiro_seguro("Quantidade de pessoas na mesa (1-20): ", 1, 20);

        int inicio_pedidos_mesa = total_pedidos;
        int continuar = 1;

        do {
            char item[50];
            int tipo, qtd;

            mid_line();
            do {
                printf("Item que deseja pedir (letras e espacos apenas): ");
                fgets(item, sizeof(item), stdin);
                item[strcspn(item, "\n")] = '\0';
                if (!validar_nome_item(item)) {
                    printf("\033[1;31mNome invalido! Use apenas letras e espacos.\033[0m\n");
                }
            } while (!validar_nome_item(item));

            tipo = ler_inteiro_seguro("Tipo do item (1 = Comida, 2 = Bebida): ", 1, 2);
            qtd = ler_inteiro_seguro("Quantidade: ", 1, 1000);

            if (total_pedidos >= capacidade) {
                capacidade = (capacidade == 0) ? 10 : capacidade * 2;
                struct dados_pedido *temp_ptr = realloc(pedidos, capacidade * sizeof(struct dados_pedido));
                if (temp_ptr == NULL) {
                    printf("Erro de alocacao de memoria.\n");
                    free(pedidos);
                    return 1;
                }
                pedidos = temp_ptr;
            }

            registrar_e_salvar(pedidos, &total_pedidos, mesa, pessoas, item, tipo, qtd, "", 0.0);

            continuar = validar_resposta_sn("Deseja pedir mais alguma coisa?");
        } while (continuar);

        system("cls || clear");

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

        char codigo_cupom[20] = "";
        float desconto = 0.0;

        if (validar_resposta_sn("Deseja aplicar cupom de desconto para essa mesa?")) {
            while (1) {
                printf("Digite o codigo do cupom (ou 'n' para cancelar): ");
                fgets(codigo_cupom, sizeof(codigo_cupom), stdin);
                codigo_cupom[strcspn(codigo_cupom, "\n")] = '\0';

                if (strlen(codigo_cupom) == 1 && tolower(codigo_cupom[0]) == 'n') {
                    printf("\033[1;33mCupom cancelado pelo usuario.\033[0m\n");
                    strcpy(codigo_cupom, "");
                    desconto = 0.0;
                    break;
                }

                if (!aplicar_cupom(codigo_cupom, cupons, &desconto)) {
                    printf("\033[1;31mCupom invalido! Tente novamente ou digite 'n' para cancelar.\033[0m\n");
                } else {
                    printf("\033[1;32mCupom valido! Desconto de %.0f%% aplicado.\033[0m\n", desconto * 100);
                    break;
                }
            }

            for (int i = inicio_pedidos_mesa; i < total_pedidos; i++) {
                if (pedidos[i].mesa == mesa) {
                    strcpy(pedidos[i].cupom, codigo_cupom);
                    pedidos[i].desconto = desconto;
                }
            }

            FILE *arquivo = fopen(ARQUIVO_BINARIO, "wb");
            if (arquivo != NULL) {
                fwrite(pedidos, sizeof(struct dados_pedido), total_pedidos, arquivo);
                fclose(arquivo);
            } else {
                printf("\033[1;31mErro ao salvar os pedidos no arquivo.\033[0m\n");
            }
        }

        float total_final = 0.0;
        for (int i = inicio_pedidos_mesa; i < total_pedidos; i++) {
            total_final += pedidos[i].subtotal * (1 - pedidos[i].desconto);
        }

        mid_line();
        printf("TOTAL FINAL DA MESA %d: R$%.2f | POR PESSOA: R$%.2f\n",
               mesa, total_final, total_final / pessoas);
        top_bottom();

        continuar_programa = validar_resposta_sn("Deseja registrar pedidos de outra mesa?");
        system("cls || clear");

    } while (continuar_programa);

    if (validar_resposta_sn("Deseja visualizar o relatorio final do dia?")) {
        system("cls || clear");
        gerar_relatorio_final();
    }

    free(pedidos);
    return 0;
}
