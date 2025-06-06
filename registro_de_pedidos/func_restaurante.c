#include "func_restaurante.h"
#include <stdio.h>
#include <stdlib.h>

// Função de registrar um item no cardápio
//    -> Salva um registro do tipo Cardapio ao final do arquivo binário padrão
//    -> (Pendente) Restaurates podem ter mais de um cardápio para diferentes dias/horários
//    -> (Pendente) Permite escolher em qual cardápio será feito o registro (arquivos binários)

int adicionar_item(Cardapio item) {

  // Abre o arquivo binário padrão para adicionar ao fim. Se não existe, cria o arquivo "cardapio.bin"
	FILE *fptr = fopen (STD_BIN, "ab");

  // Verifica se o arquivo foi acessado para escrita. Em caso de falha retorna 1
  if (fptr == NULL) {

    printf("Falha ao tentar abrir cardapio para adicionar item.\n");

    return 1;

  }

  // Escreve uma item no cardaio. Em caso de falha, retorna 1
  if(fwrite(&item, sizeof(Cardapio), 1, fptr) != 1) {

    printf("Falha ao adicionar item ao cardapio,\n");

    return 1;

  }

  // Ao fim, fecha o arquivo binário
  fclose(fptr);

  // Em caso de sucesso, retorna 0
  return 0;

}

// Função para carregar todo o cardapio para a memoria em um array de itens do tipo Cardapio (passagem por referencia)
// Retorna 1 se bem sucedido, caso contrário retorna 0

int carregar_cardapio(char *filename, Cardapio **cardapio) {
    if (!filename || !cardapio) {
        printf("Erro: Parâmetros inválidos\n");
        return 1;
    }

    FILE *fptr = fopen(filename, "rb");
    if (!fptr) {
        printf("Erro ao abrir arquivo %s\n", filename);
        return 1;
    }

    if (fseek(fptr, 0, SEEK_END) != 0) {
        printf("Erro ao navegar no arquivo\n");
        fclose(fptr);
        return 1;
    }

    long size = ftell(fptr);
    if (size == -1L) {
        printf("Erro ao obter tamanho do arquivo\n");
        fclose(fptr);
        return 1;
    }

    rewind(fptr);

    if (size % sizeof(Cardapio) != 0) {
        printf("Erro: Tamanho do arquivo inválido\n");
        fclose(fptr);
        return 1;
    }

    *cardapio = malloc(size);
    if (!*cardapio) {
        printf("Erro ao alocar %ld bytes\n", size);
        fclose(fptr);
        return 1;
    }

    size_t itens_esperados = size / sizeof(Cardapio);
    size_t itens_lidos = fread(*cardapio, sizeof(Cardapio), itens_esperados, fptr);

    if (ferror(fptr)) {
        printf("Erro na leitura\n");
        free(*cardapio);
        fclose(fptr);
        return 1;
    }

    if (itens_lidos != itens_esperados) {
        printf("Aviso: Ler %lu itens (esperado %lu)\n",
               (unsigned long)itens_lidos, (unsigned long)itens_esperados);
    }

    if (fclose(fptr) != 0) {
        printf("Aviso: Problema ao fechar arquivo\n");
    }

    return (itens_lidos == itens_esperados) ? 0 : 1;
}

int imprimir_cardapio(char *filename) {

  FILE *fptr = fopen(filename, "rb");

  Cardapio item_atual;

  int numero_sequencial = 1;

  if (fptr == NULL) {

    printf("Falha ao tentar abrir cardapio para leitura.\n");

    return 1;

  }

  printf("\n-------------------------- C A R D A P I O -------------------------\n");
  printf("|------|--------------------------------|-----------|--------------|\n");
  printf("| %-4s | %-30s | %-9s | %-12s |\n", "No.", "Item", "Tipo", "Valor (R$)");
  printf("|------|--------------------------------|-----------|--------------|\n");

  while (fread(&item_atual, sizeof(Cardapio), 1, fptr) == 1) {

    printf("| %-4d | %-30.30s | %-9.9s | R$ %9.2f |\n",
      numero_sequencial,
      item_atual.item,
      tipo_str(item_atual.tipo),
      item_atual.valor);

    numero_sequencial++;

  }

 printf("|------|--------------------------------|-----------|--------------|\n");

  return 0;

}

char* tipo_str(int tipo) {
    static char* tipos[] = {"Comida", "Bebida", "Sobremesa"};
    return (tipo >= 0 && tipo <= 2) ? tipos[tipo] : "Desconhecido";
}

int total_itens_cardapio(char *filename) {
    FILE *f = fopen(filename, "rb");
    if (!f) return 0;
    
    fseek(f, 0, SEEK_END);
    int total = ftell(f) / sizeof(Cardapio);
    fclose(f);
    return total;
}