#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "func_restaurante.h"  // Define a struct Cardapio e funções

// Lê uma string com segurança
void ler_string(const char *mensagem, char *buffer, int tamanho) {
    printf("%s: ", mensagem);
    fgets(buffer, tamanho, stdin);
    buffer[strcspn(buffer, "\n")] = '\0'; // Remove '\n'
}

// Valida se a string contém apenas letras e espaços
int validar_nome(const char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isalpha(str[i]) && str[i] != ' ') return 0;
    }
    return 1;
}

// Lê um número inteiro dentro de um intervalo
int ler_inteiro(const char *mensagem, int min, int max) {
    int valor;
    char entrada[100];

    while (1) {
        printf("%s (%d a %d): ", mensagem, min, max);
        if (!fgets(entrada, sizeof(entrada), stdin)) {
            clearerr(stdin);
            continue;
        }

        entrada[strcspn(entrada, "\n")] = '\0';
        if (sscanf(entrada, "%d", &valor) != 1 || valor < min || valor > max) {
            printf("\033[1;31mEntrada invalida! Tente novamente.\033[0m\n");
            continue;
        }

        return valor;
    }
}

// Lê um número float positivo
float ler_float_positivo(const char *mensagem) {
    float valor;
    char entrada[100];

    while (1) {
        printf("%s (ex: 12.50): ", mensagem);
        if (!fgets(entrada, sizeof(entrada), stdin)) {
            clearerr(stdin);
            continue;
        }

        entrada[strcspn(entrada, "\n")] = '\0';
        if (sscanf(entrada, "%f", &valor) != 1 || valor < 0) {
            printf("\033[1;31mDigite um valor valido!\033[0m\n");
            continue;
        }

        return valor;
    }
}

// Valida resposta s/n
int confirmar(const char *mensagem) {
    char resp[10];
    while (1) {
        printf("%s (s/n): ", mensagem);
        if (!fgets(resp, sizeof(resp), stdin)) continue;

        char c = tolower(resp[0]);
        if (c == 's') return 1;
        if (c == 'n') return 0;

        printf("\033[1;31mDigite apenas 's' ou 'n'.\033[0m\n");
    }
}

int main() {
    printf("===== Cadastro de Itens no Cardapio =====\n");

    do {
        Cardapio novo_item;

        // Nome do prato
        while (1) {
            ler_string("Nome do item", novo_item.item, sizeof(novo_item.item));
            if (!validar_nome(novo_item.item)) {
                printf("\033[1;31mNome invalido! Use apenas letras e espaços.\033[0m\n");
            } else {
                break;
            }
        }

        // Tipo do item
        printf("Tipos disponiveis:\n");
        printf("0 - Comida\n1 - Bebida\n2 - Sobremesa\n");
        novo_item.tipo = ler_inteiro("Escolha o tipo", 0, 2);

        // Valor
        novo_item.valor = ler_float_positivo("Preco do item");

        // Salvar no arquivo
        if (adicionar_item(novo_item) == 0) {
            printf("\033[1;32mItem adicionado com sucesso!\033[0m\n");
        } else {
            printf("\033[1;31mErro ao salvar o item no cardapio.\033[0m\n");
        }

    } while (confirmar("Deseja adicionar outro item?"));

    printf("\nCadastro finalizado. Cardapio salvo em '%s'\n", STD_BIN);
    return 0;
}
