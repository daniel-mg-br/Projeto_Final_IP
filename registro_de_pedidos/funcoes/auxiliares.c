/*Funções auxiliares de estética e validação de respostas*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "auxiliares.h"

// Imprime uma linha intermediária para separar conteúdos

void mid_line() {
    printf("----------------------------------------------------------\n");
}

// Imprime uma linha superior/inferior de destaque

void top_bottom() {
    printf("==========================================================\n");
}

// Lê um número inteiro seguro dentro de um intervalo definido

int ler_inteiro_seguro(const char *mensagem, int min, int max) {
    char entrada[100]; // Buffer para armazenar a linha digitada
    int valor;         // Variável onde será armazenado o número convertido

    while (1) {
        printf("%s ", mensagem);

        // Lê uma linha completa da entrada padrão (stdin), incluindo o '\n'.
        // Isso evita que um '\n' residual atrapalhe a leitura.

        if (!fgets(entrada, sizeof(entrada), stdin)) {
            clearerr(stdin); // Limpa erros no buffer, se houver
            continue;        // Reinicia o loop caso a leitura falhe
        }

        entrada[strcspn(entrada, "\n")] = '\0'; // Substitui '\n' por '\0' na string


        // Usa sscanf para tentar extrair um número inteiro da string.
        // Se falhar (não for número), emite mensagem e repete o laço.

        if (sscanf(entrada, "%d", &valor) != 1) {
            printf("\033[1;31mErro: Digite um numero valido!\033[0m\n");
            mid_line();
            continue;
        }

        // Verifica se o valor está dentro do intervalo permitido

        if (valor < min || valor > max) {
            printf("\033[1;31mErro: Valor deve estar entre %d e %d!\033[0m\n", min, max);
            mid_line();
            continue;
        }

        return valor;
    }
}

// Valida a resposta do usuário para uma pergunta de sim/não

int validar_resposta_sn(const char *mensagem) {
    char resposta; // Variável para armazenar a letra digitada
    int c;         // Limpa o restante da linha do buffer

    do {
        printf("%s (s/n): ", mensagem);

        resposta = getchar(); // Lê o primeiro caractere digitado
        while ((c = getchar()) != '\n' && c != EOF);  // Limpa os caracteres restantes no buffer

        resposta = tolower(resposta); // Converte para minúsculo

        // Se for a resposta for inválida, informa ao usuário e repete o laço

        if (resposta != 's' && resposta != 'n') {
            printf("\033[1;31mOpcao invalida! Digite 's' ou 'n'.\033[0m\n");
            mid_line();
        }

    } while (resposta != 's' && resposta != 'n');

    return (resposta == 's'); // 1 = 's' e 0 = 'n'
}

// Valida o nome do item do cardápio
int validar_nome_item(const char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        // Verifica se o caractere é uma letra ou um espaço

        if (!isalpha(str[i]) && str[i] != ' ') return 0; // Caractere inválido -> Nome inválido
    }
    return 1; // Caracteres válidos -> Nome válido
}

// Lê um número float positivo
float ler_float_positivo(const char *mensagem) {
    float valor;
    char entrada[100];

    while (1) {
        printf("%s", mensagem);

        // Lê uma linha completa da entrada padrão (stdin), incluindo o '\n'.
        if (!fgets(entrada, sizeof(entrada), stdin)) {
            clearerr(stdin); // Limpa erros no buffer, se houver
            continue;
        }

        entrada[strcspn(entrada, "\n")] = '\0';
        if (sscanf(entrada, "%f", &valor) != 1 || valor < 0) { // Verifica se a conversão foi bem-sucedida e se o valor é positivo
            printf("\033[1;31mDigite um valor valido!\033[0m\n");
            mid_line();
            continue;
        }

        return valor;
    }
}

// Aguardando o usuário pressionar Enter para continuar
void pausar_tela() {
    printf("\nPressione Enter para continuar...");
    while (getchar() != '\n'); // Limpa buffer
}
