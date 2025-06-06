#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "func_restaurante.h"
#include "funcoes.h"

// Imprime uma linha intermediária para separar conteúdos
void mid_line() {
    printf("----------------------------------------------------------\n");
}

// Imprime uma linha superior/inferior de destaque
void top_bottom() {
    printf("==========================================================\n");
}

// Inicializa o vetor de cupons com códigos e porcentagens fixas
void inicializar_cupons(struct cupom_desconto cupons[]) {
    strcpy(cupons[0].codigo, "paulo"); cupons[0].porcentagem = 0.20f;
    strcpy(cupons[1].codigo, "daniel"); cupons[1].porcentagem = 0.30f;
    strcpy(cupons[2].codigo, "pedro"); cupons[2].porcentagem = 0.40f;
    strcpy(cupons[3].codigo, "lizandro"); cupons[3].porcentagem = 0.50f;
    strcpy(cupons[4].codigo, "gean"); cupons[4].porcentagem = 0.60f;
}

// Valida se o código do cupom é válido e define a porcentagem correspondente
int aplicar_cupom(char codigo[], struct cupom_desconto cupons[], float *desconto) {
    for (int i = 0; i < MAX_CUPONS; i++) {
        if (strcasecmp(codigo, cupons[i].codigo) == 0) {
            *desconto = cupons[i].porcentagem;
            return 1;
        }
    }
    return 0;
}

// Lê e valida um inteiro dentro de um intervalo
int ler_inteiro_seguro(const char *mensagem, int min, int max) {
    char entrada[100];
    int valor;

    while (1) {
        printf("%s ", mensagem);

        if (!fgets(entrada, sizeof(entrada), stdin)) {
            clearerr(stdin);
            continue;
        }

        entrada[strcspn(entrada, "\n")] = '\0';

        if (sscanf(entrada, "%d", &valor) != 1) {
            printf("\033[1;31mErro: Digite um número válido!\033[0m\n");
            continue;
        }

        if (valor < min || valor > max) {
            printf("\033[1;31mErro: Valor deve estar entre %d e %d!\033[0m\n", min, max);
            continue;
        }

        return valor;
    }
}



// Valida entrada s/n com tratamento de erro
int validar_resposta_sn(const char *mensagem) {
    char resposta;
    int c;

    do {
        printf("%s (s/n): ", mensagem);

        resposta = getchar();
        while ((c = getchar()) != '\n' && c != EOF);  // Limpa o restante da linha

        resposta = tolower(resposta);

        if (resposta != 's' && resposta != 'n') {
            printf("\033[1;31mOpcao invalida! Digite 's' ou 'n'.\033[0m\n");
        }

    } while (resposta != 's' && resposta != 'n');

    return (resposta == 's');
}




// Valida se uma string contém apenas letras e espaços
int validar_nome_item(const char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isalpha(str[i]) && str[i] != ' ') return 0;
    }
    return 1;
}

Cardapio* buscar_item_cardapio(int posicao, Cardapio *cardapio, int total_itens) {
    if (posicao < 1 || posicao > total_itens) {
        printf("\033[1;31mErro: Posicao %d invalida no cardapio!\033[0m\n", posicao);
        return NULL;
    }
    return &cardapio[posicao - 1]; // Ajuste para índice 0-based
}

// Registra o pedido na memória e salva no arquivo binário
void registrar_e_salvar(
    struct dados_pedido **vetor,       // ponteiro duplo
    int *total_pedidos,
    int *capacidade,
    int mesa,
    int pessoas,
    int posicao_item,
    Cardapio *cardapio,
    int total_itens_cardapio,
    int quantidade,
    const char *cupom,
    float desconto) {
    
    if (!cardapio || posicao_item < 1 || posicao_item > total_itens_cardapio) {
        printf("\033[1;31mErro: Item invalido ou cardapio nao carregado!\033[0m\n");
        return;
    }

    // Realocar vetor se necessário
    if (*total_pedidos >= *capacidade) {
        *capacidade = (*capacidade == 0) ? 10 : (*capacidade * 2);
        struct dados_pedido *temp = realloc(*vetor, (*capacidade) * sizeof(struct dados_pedido));
        if (!temp) {
            printf("\033[1;31mErro ao realocar memoria para pedidos!\033[0m\n");
            return;
        }
        *vetor = temp;
    }

    Cardapio *item = &cardapio[posicao_item - 1];  // índice ajustado

    struct dados_pedido pedido = {
        .mesa = mesa,
        .pessoas = pessoas,
        .tipo = item->tipo,
        .quantidade = quantidade,
        .subtotal = quantidade * item->valor,
        .valor_individual = (quantidade * item->valor) / (pessoas > 0 ? pessoas : 1),
        .desconto = desconto
    };

    strncpy(pedido.item, item->item, sizeof(pedido.item) - 1);
    strncpy(pedido.cupom, cupom, sizeof(pedido.cupom) - 1);

    (*vetor)[*total_pedidos] = pedido;
    (*total_pedidos)++;

    FILE *arquivo = fopen(ARQUIVO_BINARIO, "ab");
    if (arquivo) {
        fwrite(&pedido, sizeof(struct dados_pedido), 1, arquivo);
        fclose(arquivo);
    } else {
        printf("\033[1;31mErro ao salvar pedido no arquivo!\033[0m\n");
    }
}


// Carrega pedidos do arquivo binário para um vetor alocado dinamicamente
int carregar_pedidos(struct dados_pedido **vetor, int *total_pedidos, int *capacidade) {
    FILE *arquivo = fopen(ARQUIVO_BINARIO, "rb");
    if (arquivo == NULL) {
        return 0;
    }

    struct dados_pedido temp;
    while (fread(&temp, sizeof(struct dados_pedido), 1, arquivo) == 1) {
        if (*total_pedidos >= *capacidade) {
            *capacidade = (*capacidade == 0) ? 10 : (*capacidade * 2);
            struct dados_pedido *temp_ptr = realloc(*vetor, (*capacidade) * sizeof(struct dados_pedido));
            if (temp_ptr == NULL) {
                printf("Erro ao alocar memoria para carregar pedidos.\n");
                fclose(arquivo);
                return 0;
            }
            *vetor = temp_ptr;
        }
        (*vetor)[(*total_pedidos)++] = temp;
    }

    fclose(arquivo);
    return 1;
}

// Exibe os dados de um pedido formatadamente
void exibir_pedido(struct dados_pedido p, int eh_relatorio) {
    printf("Mesa %d | Pessoas: %d\n", p.mesa, p.pessoas);
    printf("Item: %s (%dx) [%s]\n", p.item, p.quantidade, tipo_str(p.tipo)); // Usa tipo_str do colega
    printf("Subtotal: R$%.2f\n", p.subtotal);
    if (strlen(p.cupom) > 0 && !eh_relatorio) {
        printf("CUPOM %s APLICADO (%.0f%% OFF)\n", p.cupom, p.desconto * 100);
        printf("VALOR COM DESCONTO: R$%.2f\n", p.subtotal * (1 - p.desconto));
    }
    mid_line();
}

// Gera o relatório geral do sistema com total por mesa e ranking de vendas
void gerar_relatorio_final() {
    struct dados_pedido *pedidos = NULL;
    int total_pedidos = 0;
    int capacidade = 0;

    if (!carregar_pedidos(&pedidos, &total_pedidos, &capacidade)) {
        printf("Nenhum pedido registrado ainda.\n");
        return;
    }

    calcular_metricas_gerais(pedidos, total_pedidos);

    int mesas[100] = {0}, total_mesas = 0;
    for (int i = 0; i < total_pedidos; i++) {
        int existe = 0;
        for (int j = 0; j < total_mesas; j++) {
            if (mesas[j] == pedidos[i].mesa) {
                existe = 1;
                break;
            }
        }
        if (!existe) mesas[total_mesas++] = pedidos[i].mesa;
    }

    for (int i = 0; i < total_mesas; i++) {
        int mesa = mesas[i];
        float total_original = 0, total_final = 0;
        char cupom[20] = "";
        float desconto = 0;
        int pessoas = 0;

        printf("\nMESA %d\n", mesa);
        mid_line();
        for (int j = 0; j < total_pedidos; j++) {
            if (pedidos[j].mesa == mesa) {
                exibir_pedido(pedidos[j], 1);
                total_original += pedidos[j].subtotal;
                if (strlen(pedidos[j].cupom) > 0) {
                    strcpy(cupom, pedidos[j].cupom);
                    desconto = pedidos[j].desconto;
                }
                pessoas = pedidos[j].pessoas;
            }
        }
        total_final = total_original * (1 - desconto);
        printf("\nRESUMO FINANCEIRO:\n");
        printf("VALOR TOTAL:    R$%.2f\n", total_original);
        if (strlen(cupom) > 0) {
            printf("CUPOM %-8s   -%.0f%%\n", cupom, desconto * 100);
            printf("VALOR FINAL:    R$%.2f\n", total_final);
        }
        printf("POR PESSOA:     R$%.2f\n", total_final / pessoas);
        top_bottom();
    }

    gerar_ranking(pedidos, total_pedidos);
    calcular_subtotal_por_tipo(pedidos, total_pedidos);

    free(pedidos);
}

// Calcula totais gerais: arrecadação, média por mesa e total de itens
void calcular_metricas_gerais(struct dados_pedido pedidos[], int total_pedidos) {
    float total_arrecadado = 0;
    int total_itens = 0;
    int mesas[100] = {0}, total_mesas = 0;

    for (int i = 0; i < total_pedidos; i++) {
        total_arrecadado += pedidos[i].subtotal;
        total_itens += pedidos[i].quantidade;
        int existe = 0;
        for (int j = 0; j < total_mesas; j++) {
            if (mesas[j] == pedidos[i].mesa) {
                existe = 1;
                break;
            }
        }
        if (!existe) mesas[total_mesas++] = pedidos[i].mesa;
    }

    printf("\n=== RESUMO GERAL ===\n");
    top_bottom();
    printf("TOTAL ARRECADADO: R$ %10.2f\n", total_arrecadado);
    printf("MEDIA POR MESA:  R$ %10.2f\n", total_mesas > 0 ? total_arrecadado / total_mesas : 0);
    printf("ITENS VENDIDOS:  %10d unidades\n", total_itens);
    top_bottom();
}

// Separa e exibe o subtotal arrecadado por tipo (comida ou bebida)
void calcular_subtotal_por_tipo(struct dados_pedido pedidos[], int total_pedidos) {
    float total_pratos = 0, total_bebidas = 0, total_sobremesas = 0;

    for (int i = 0; i < total_pedidos; i++) {
        switch(pedidos[i].tipo) {
            case 0: total_pratos += pedidos[i].subtotal; break;
            case 1: total_bebidas += pedidos[i].subtotal; break;
            case 2: total_sobremesas += pedidos[i].subtotal; break;
        }
    }

    printf("\n=== SUBTOTAL POR CATEGORIA ===\n");
    top_bottom();
    printf("PRATOS:     R$ %8.2f\n", total_pratos);
    printf("BEBIDAS:    R$ %8.2f\n", total_bebidas);
    printf("SOBREMESAS: R$ %8.2f\n", total_sobremesas);
    printf("TOTAL:      R$ %8.2f\n", total_pratos + total_bebidas + total_sobremesas);
    top_bottom();
}

// Cria ranking dos itens mais vendidos com base na quantidade
void gerar_ranking(struct dados_pedido pedidos[], int total_pedidos) {
    struct item_ranking {
        char nome[50];
        int quantidade;
        float subtotal;
    } ranking[50];

    int n_itens = 0;
    for (int i = 0; i < total_pedidos; i++) {
        int encontrado = 0;
        for (int j = 0; j < n_itens; j++) {
            if (strcmp(pedidos[i].item, ranking[j].nome) == 0) {
                ranking[j].quantidade += pedidos[i].quantidade;
                ranking[j].subtotal += pedidos[i].subtotal;
                encontrado = 1;
                break;
            }
        }
        if (!encontrado && n_itens < 50) {
            strcpy(ranking[n_itens].nome, pedidos[i].item);
            ranking[n_itens].quantidade = pedidos[i].quantidade;
            ranking[n_itens].subtotal = pedidos[i].subtotal;
            n_itens++;
        }
    }

    for (int i = 0; i < n_itens - 1; i++) {
        for (int j = i + 1; j < n_itens; j++) {
            if (ranking[j].quantidade > ranking[i].quantidade) {
                struct item_ranking temp = ranking[i];
                ranking[i] = ranking[j];
                ranking[j] = temp;
            }
        }
    }

    printf("\n=== TOP 5 ITENS MAIS VENDIDOS ===\n");
    top_bottom();
    for (int i = 0; i < (n_itens < 5 ? n_itens : 5); i++) {
        printf("%do: %-20s | %3dx | R$ %7.2f\n", i + 1, ranking[i].nome,
               ranking[i].quantidade, ranking[i].subtotal);
    }
    top_bottom();
}