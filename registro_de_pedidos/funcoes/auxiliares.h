// Funções auxiliares de estética e validação de respostas

#pragma once

// Imprime uma linha intermediária para separar conteúdos
void mid_line(); 

// Imprime uma linha superior/inferior de destaque
void top_bottom();

// Função que solicita ao usuário um número inteiro dentro de um intervalo válido.
int ler_inteiro_seguro(const char *mensagem, int min, int max);

// Valida se a resposta do usuário é "sim" ou "não" (ou suas variações).
int validar_resposta_sn(const char *mensagem);

// Valida se a string é um nome de item válido (não vazio e sem caracteres especiais).
int validar_nome_item(const char *str);

// Lê um número float positivo do usuário, garantindo que seja válido.
float ler_float_positivo(const char *mensagem);

// Aguardando o usuário pressionar Enter para continuar
void pausar_tela();