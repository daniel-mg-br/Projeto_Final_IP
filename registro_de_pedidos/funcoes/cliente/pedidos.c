#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../auxiliares.h"
#include "../restaurante/func-restaurante.h"
#include "func-cliente.h"

// Função que inicializa o vetor de cupons com alguns valores pré-definidos
void inicializar_cupons(Cupom* cupons, int* total) {
    *total = 5;
    
    strcpy(cupons[0].codigo, "desc40");
    cupons[0].desconto = 0.4;

    strcpy(cupons[1].codigo, "desc10");
    cupons[1].desconto = 0.1;

    strcpy(cupons[2].codigo, "desc20");
    cupons[2].desconto = 0.2;

    strcpy(cupons[3].codigo, "desc15");
    cupons[3].desconto = 0.15;

    strcpy(cupons[4].codigo, "desc5");
    cupons[4].desconto = 0.05;
}

// Função que valida o cupom digitado pelo usuário.
// Percorre o vetor de cupons e verifica se o código digitado corresponde a algum cupom válido.
int validar_cupom(const char* entrada, Cupom* cupons, int total, float* desconto) {
    for (int i = 0; i < total; i++) {
        if (strcmp(entrada, cupons[i].codigo) == 0) { // Verifica se o código do cupom é igual ao digitado
            *desconto = cupons[i].desconto;
            return 1; // Cupom válido
        }
    }
    return 0; // Inválido
}

// Função responsável por registrar os pedidos dos clientes.
// Permite que o usuário registre pedidos de diferentes mesas, adicionando itens do cardápio
// e aplicando cupons de desconto, se desejado.
void realizar_pedido() {
    int deseja_outra_mesa = 1;

    while (deseja_outra_mesa) { // Loop para registrar pedidos de várias mesas
        system("clear || cls");
        top_bottom();
        printf("REGISTRO DE PEDIDOS\n");
        mid_line();

        int mesa = ler_inteiro_seguro("Digite o numero da mesa:", 1, 100);
        int pessoas = ler_inteiro_seguro("Quantidade de pessoas na mesa:", 1, 20);

        Pedido* pedidos_da_mesa = NULL; // Inicializa o vetor de pedidos da mesa como NULL
        int pedidos_count = 0; // Contador de pedidos registrados na mesa
        float total_mesa = 0; // Inicializa o total da mesa como 0

        int deseja_mais_itens = 1; // Variável para controlar se o usuário deseja adicionar mais itens

        while (deseja_mais_itens) { // Loop para adicionar itens à mesa
            exibir_cardapio();

            int total_cardapio; 

            ItemCardapio* cardapio = carregar_cardapio(&total_cardapio);

            // Verifica se o cardápio foi carregado corretamente
            if (!cardapio || total_cardapio == 0) {
                printf("\033[1;31mCardapio indisponivel.\033[0m\n");
                return;
            }

            int pos = ler_inteiro_seguro("Digite a posicao do item desejado:", 1, total_cardapio);
            pos--;

            // Verifica se a posição é válida
            if (!cardapio[pos].ativo) {
                printf("\033[1;31mItem inativo!\033[0m\n");
                mid_line();
                liberar_cardapio(cardapio);
                continue;
            }

            int qtd = ler_inteiro_seguro("Digite a quantidade desejada:", 1, 100);
            float subtotal = cardapio[pos].valor * qtd;

            // Atribui os valores ao novo pedido
            Pedido novo;
            novo.mesa = mesa;
            novo.pessoas = pessoas;
            novo.tipo = cardapio[pos].tipo;
            novo.quantidade = qtd;
            novo.subtotal = subtotal;
            novo.desconto_aplicado = 0.0;
            strcpy(novo.nome_item, cardapio[pos].nome);
            strcpy(novo.cupom, "");

            // Aloca memória para o vetor de pedidos da mesa
            Pedido* temp = realloc(pedidos_da_mesa, (pedidos_count + 1) * sizeof(Pedido));

            // Verifica se a alocação foi bem-sucedida
            if (!temp) {
                printf("\033[1;31mErro de alocacao de memoria.\033[0m\n");
                free(pedidos_da_mesa);
                liberar_cardapio(cardapio);
                return;
            }

            pedidos_da_mesa = temp;
            pedidos_da_mesa[pedidos_count++] = novo;

            total_mesa += subtotal;
            liberar_cardapio(cardapio);

            deseja_mais_itens = validar_resposta_sn("Deseja adicionar mais itens a esta mesa?");
        }

        // Exibir resumo
        system("clear || cls");
        top_bottom();
        printf("RESUMO DOS PEDIDOS DA MESA %d:\n", mesa);
        top_bottom();

        // Imprime os pedidos da mesa, incluindo tipo, quantidade e subtotal

        for (int i = 0; i < pedidos_count; i++) {
            printf("Mesa %d | Pessoas: %d\n", pedidos_da_mesa[i].mesa, pedidos_da_mesa[i].pessoas);
            printf("Item: %s (%dx) [%s]\n",
                   pedidos_da_mesa[i].nome_item,
                   pedidos_da_mesa[i].quantidade,
                   pedidos_da_mesa[i].tipo == COMIDA ? "Comida" :
                   pedidos_da_mesa[i].tipo == BEBIDA ? "Bebida" : "Sobremesa");
            printf("Subtotal: R$%.2f\n", pedidos_da_mesa[i].subtotal);
            mid_line();
        }

        printf("TOTAL DA MESA (sem desconto): R$%.2f\n", total_mesa);

        // Inicializar cupons
        Cupom cupons[MAX_CUPONS];
        int total_cupons = 0;
        inicializar_cupons(cupons, &total_cupons);

        // Aplicar cupom
        if (validar_resposta_sn("Deseja aplicar cupom de desconto?")) {
            while (1) {
                char entrada[20];
                printf("Digite o codigo do cupom (ou 'n' para cancelar): ");
                fgets(entrada, sizeof(entrada), stdin); // Lê a entrada do usuário
                entrada[strcspn(entrada, "\n")] = '\0';

                if (strcmp(entrada, "n") == 0) break; // Se o usuário digitar 'n', sai do loop

                float desconto = 0.0;

                // Verifica se o cupom é válido
                if (validar_cupom(entrada, cupons, total_cupons, &desconto)) {
                    float total_final = total_mesa * (1 - desconto); // Calcula o total final aplicando o desconto
                    float por_pessoa = total_final / pessoas; // Calcula o valor por pessoa

                    printf("\033[1;32mCupom '%s' valido! Desconto de %.0f%% aplicado.\033[0m\n", entrada, desconto * 100);
                    mid_line();

                    printf("TOTAL FINAL DA MESA %d: R$%.2f\n", mesa, total_final);
                    printf("VALOR POR PESSOA: R$%.2f\n", por_pessoa);

                    // Aplica o desconto a todos os pedidos da mesa

                    for (int i = 0; i < pedidos_count; i++) {
                        pedidos_da_mesa[i].desconto_aplicado = desconto;
                        strcpy(pedidos_da_mesa[i].cupom, entrada);
                    }

                    break;
                } else {
                    printf("\033[1;31mCupom invalido!\033[0m\n");
                    if (!validar_resposta_sn("Deseja tentar outro cupom?")) break;
                }
            }
        }

        top_bottom();

        // Salvar os pedidos
        FILE* f = fopen(PEDIDO_DB, "ab"); // Abre o arquivo em modo de adição binária
        if (f) {
            fwrite(pedidos_da_mesa, sizeof(Pedido), pedidos_count, f);
            fclose(f);
        } else {
            printf("\033[1;31mErro ao salvar pedidos.\033[0m\n");
        }

        free(pedidos_da_mesa); // Libera a memória alocada para os pedidos da mesa

        deseja_outra_mesa = validar_resposta_sn("Deseja registrar pedidos de outra mesa?");

        system("clear || cls");
        top_bottom();

    }
    system("clear || cls");
    top_bottom();
}

// Função que gera um relatório dos pedidos registrados.
// Exibe um resumo geral, incluindo total arrecadado, média por mesa, itens mais vendidos,
// e um detalhamento por mesa, incluindo subtotais e descontos aplicados.

void gerar_relatorio() {

    // Abre o arquivo de pedidos em modo leitura binária
    FILE* f = fopen("pedidos.bin", "rb");
    if (!f) {
        printf("\033[1;31mNenhum pedido registrado ainda.\033[0m\n");
        pausar_tela();
        return;
    }

    // Aloca memória para armazenar os pedidos
    Pedido* pedidos = malloc(sizeof(Pedido) * 1000);

    // Verifica se a alocação foi bem-sucedida
    if (!pedidos) {
        fclose(f);
        printf("Erro de memoria.\n");
        return;
    }

    // Lê os pedidos do arquivo
    int total_pedidos = fread(pedidos, sizeof(Pedido), 1000, f);
    fclose(f);

    // Verifica se algum pedido foi lido
    if (total_pedidos == 0) {
        printf("\033[1;31mArquivo de pedidos vazio.\033[0m\n");
        free(pedidos);
        pausar_tela();
        return;
    }

    system("clear || cls");
    top_bottom();
    printf("=== RESUMO GERAL ===\n");
    top_bottom();

    float total_arrecadado = 0; // Inicializa o total arrecadado como 0
    int mesas_unicas[100] = {0}; // Array para armazenar mesas únicas
    int total_mesas = 0; // Contador de mesas únicas
    int total_itens = 0; // Contador de itens vendidos

    ItemEstat itens_mais_vendidos[100]; // Array para armazenar os itens mais vendidos
    int total_itens_estat = 0; // Contador de itens estatísticos

    // Começa a calcular o total arrecadado, mesas únicas, itens vendidos e estatísticas
    for (int i = 0; i < total_pedidos; i++) {
        Pedido p = pedidos[i];
        total_arrecadado += p.subtotal * (1 - p.desconto_aplicado);
        total_itens += p.quantidade;

        // Verifica se a mesa já foi contabilizada
        int nova_mesa = 1;
        for (int j = 0; j < total_mesas; j++) {
            if (mesas_unicas[j] == p.mesa) {
                nova_mesa = 0;
                break;
            }
        }

        // Se for uma nova mesa, adiciona ao array de mesas únicas
        if (nova_mesa) mesas_unicas[total_mesas++] = p.mesa;

        int encontrado = 0;

        // Verifica se o item já está contabilizado nos itens mais vendidos
        for (int j = 0; j < total_itens_estat; j++) {
            if (strcmp(itens_mais_vendidos[j].nome, p.nome_item) == 0) {
                itens_mais_vendidos[j].quantidade += p.quantidade;
                itens_mais_vendidos[j].total += p.subtotal;
                encontrado = 1;
                break;
            }
        }

        // Se o item não foi encontrado, adiciona um novo registro
        if (!encontrado) {
            strcpy(itens_mais_vendidos[total_itens_estat].nome, p.nome_item);
            itens_mais_vendidos[total_itens_estat].quantidade = p.quantidade;
            itens_mais_vendidos[total_itens_estat].total = p.subtotal;
            total_itens_estat++;
        }
    }

    printf("TOTAL ARRECADADO: R$%.2f\n", total_arrecadado);
    printf("MEDIA POR MESA:   R$%.2f\n", total_arrecadado / total_mesas);
    printf("ITENS VENDIDOS:   %d unidades\n", total_itens);
    top_bottom();


    // Exibe o resumo financeiro por mesa
    // Percorre as mesas únicas e exibe os pedidos de cada uma
    for (int i = 0; i < total_mesas; i++) {
        int mesa = mesas_unicas[i];
        float total_mesa = 0;
        float desconto = 0;
        char cupom[20] = "";
        int pessoas = 1;

        printf("MESA %d\n", mesa);

        // Percorre todos os pedidos e exibe os itens da mesa atual
        for (int j = 0; j < total_pedidos; j++) {
            Pedido p = pedidos[j];

            // Verifica se o pedido pertence à mesa atual
            // Se sim, exibe os detalhes do pedido
            if (p.mesa == mesa) {
                pessoas = p.pessoas;
                printf("Item: %s (%dx) [%s]\n",
                       p.nome_item, p.quantidade,
                       p.tipo == COMIDA ? "Comida" :
                       p.tipo == BEBIDA ? "Bebida" : "Sobremesa");
                printf("Subtotal: R$%.2f\n", p.subtotal);
                total_mesa += p.subtotal;

                if (p.desconto_aplicado > 0) {
                    desconto = p.desconto_aplicado;
                    strcpy(cupom, p.cupom);
                }
            }
        }

        mid_line();

        // Exibe o resumo financeiro da mesa
        // Se houver desconto, exibe o cupom e o valor final
        printf("RESUMO FINANCEIRO:\n");
        printf("VALOR TOTAL: R$%.2f\n", total_mesa);

        // Verifica se há desconto aplicado
        if (desconto > 0) {
            printf("CUPOM: %s (%.0f%%)\n", cupom, desconto * 100);
            float final = total_mesa * (1 - desconto);
            printf("VALOR FINAL: R$%.2f\n", final); // Exibe o valor final após desconto
            printf("POR PESSOA:  R$%.2f\n", final / pessoas); // Exibe o valor por pessoa após desconto
        } else {
            printf("SEM DESCONTO APLICADO\n");
            printf("POR PESSOA:  R$%.2f\n", total_mesa / pessoas);
        }

        top_bottom();
    }

    // Exibe os itens mais vendidos
    // Ordena os itens mais vendidos por quantidade
    printf("=== TOP 5 ITENS MAIS VENDIDOS ===\n");

    // Bubble sort para ordenar os itens mais vendidos
    for (int i = 0; i < total_itens_estat - 1; i++) {
        for (int j = i + 1; j < total_itens_estat; j++) {
            if (itens_mais_vendidos[j].quantidade > itens_mais_vendidos[i].quantidade) {
                ItemEstat tmp = itens_mais_vendidos[i];
                itens_mais_vendidos[i] = itens_mais_vendidos[j];
                itens_mais_vendidos[j] = tmp;
            }
        }
    }

    // Exibe os 5 itens mais vendidos
    for (int i = 0; i < total_itens_estat && i < 5; i++) {
        printf("%do: %-15s | %2dx | R$ %6.2f\n", i+1,
               itens_mais_vendidos[i].nome,
               itens_mais_vendidos[i].quantidade,
               itens_mais_vendidos[i].total);
    }

    top_bottom();

    float comida = 0, bebida = 0, sobremesa = 0;

    // Calcula o subtotal por categoria
    for (int i = 0; i < total_pedidos; i++) {
        float valor_final = pedidos[i].subtotal * (1 - pedidos[i].desconto_aplicado); // Aplica o desconto ao subtotal
        if (pedidos[i].tipo == COMIDA) comida += valor_final; // Adiciona ao subtotal de comida
        else if (pedidos[i].tipo == BEBIDA) bebida += valor_final; // Adiciona ao subtotal de bebida
        else if (pedidos[i].tipo == SOBREMESA) sobremesa += valor_final; // Adiciona ao subtotal de sobremesa
        else sobremesa += valor_final; 
    }

    // Exibe o subtotal por categoria
    printf("=== SUBTOTAL POR CATEGORIA ===\n");
    printf("PRATOS:     R$ %.2f\n", comida);
    printf("BEBIDAS:    R$ %.2f\n", bebida);
    printf("SOBREMESAS: R$ %.2f\n", sobremesa);
    printf("TOTAL:      R$ %.2f\n", total_arrecadado);
    top_bottom();

    free(pedidos); // Libera a memória alocada para os pedidos

    pausar_tela();
    
    system("clear || cls");
    top_bottom();

}