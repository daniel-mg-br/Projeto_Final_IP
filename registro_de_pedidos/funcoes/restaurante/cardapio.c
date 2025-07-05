#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "func-restaurante.h"
#include "../auxiliares.h"

/* Função auxiliar para verificar existência do arquivo */
static bool arquivo_existe(const char* filename) {
    return access(filename, F_OK) == 0; // Verifica se o arquivo existe
}

// Verifica se o usuário tem permissão para modificar o cardápio
static bool verificar_permissoes() {
    if (arquivo_existe(CARDAPIO_DB)) { // Se o arquivo já existe, verifica permissões
        if (access(CARDAPIO_DB, W_OK) != 0) { // Verifica se o arquivo é gravável
            printf("\033[1;31mErro: Sem permissao para modificar o cardapio!\033[0m\n");
            mid_line();
            return false;
        }
    }
    return true;
}

/* Carrega o cardápio do arquivo binário */
ItemCardapio* carregar_cardapio(int* total_itens) {
    *total_itens = 0;
    
    // Verifica se o arquivo existe
    if (!arquivo_existe(CARDAPIO_DB)) {
        return NULL;
    }

    // Abre o arquivo para leitura binária    
    FILE* file = fopen(CARDAPIO_DB, "rb");

    // Verifica se o arquivo foi aberto corretamente
    if (!file) {
        printf("\033[1;31mErro ao abrir arquivo do cardapio!\033[0m\n");
        mid_line(); 
        return NULL;
    }

    // Obtém tamanho do arquivo
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    // Verifica se o arquivo está vazio
    if (file_size == 0) {
        fclose(file);
        return NULL;
    }

    // Calcula número de itens
    *total_itens = file_size / sizeof(ItemCardapio);
    if (*total_itens == 0) {
        fclose(file);
        return NULL;
    }

    // Aloca memória
    ItemCardapio* itens = (ItemCardapio*)malloc(file_size);

    // Verifica se a alocação foi bem-sucedida
    if (!itens) {
        fclose(file);
        *total_itens = 0;

        // Mensagem de erro com formatação
        printf("\033[1;31mErro ao alocar memoria!\033[0m\n");
        mid_line();
        return NULL;
    }

    // Carrega dados
    size_t lidos = fread(itens, sizeof(ItemCardapio), *total_itens, file);
    fclose(file);

    // Verifica se a leitura foi bem-sucedida
    if (lidos != *total_itens) {
        free(itens);
        *total_itens = 0;

        // Mensagem de erro com formatação
        printf("\033[1;31mErro ao ler cardapio!\033[0m\n");
        mid_line();
        return NULL;
    }

    return itens;
}

/* Salva o cardápio no arquivo binário */
bool salvar_cardapio(ItemCardapio* itens, int total) {
    if (!itens || total <= 0) {
        return false;
    }

    // Remove arquivo temporário residual se existir
    if (arquivo_existe(TEMP_DB)) {
        remove(TEMP_DB);
    }

    // Cria arquivo com permissões adequadas (0644 = rw-r--r--)
    FILE* file = fopen(TEMP_DB, "wb");

    // Verifica se o arquivo foi criado corretamente
    if (!file) {
        printf("\033[1;31mErro ao criar arquivo temporario! Permissao negada.\033[0m\n");
        mid_line();
        return false;
    }

    // Escreve itens no arquivo
    size_t escritos = fwrite(itens, sizeof(ItemCardapio), total, file);
    fclose(file);

    // Verifica se a escrita foi bem-sucedida
    if (escritos != total) {
        remove(TEMP_DB);

        // Mensagem de erro com formatação
        printf("\033[1;31mErro ao escrever no arquivo!\033[0m\n");
        mid_line();
        return false;
    }

    // Remove arquivo antigo se existir
    if (arquivo_existe(CARDAPIO_DB)) {
        if (remove(CARDAPIO_DB) != 0) {
            remove(TEMP_DB);

            // Mensagem de erro com formatação
            printf("\033[1;31mErro ao remover arquivo antigo! Permissao negada.\033[0m\n");
            mid_line();
            return false;
        }
    }

    // Renomeia arquivo temporário
    if (rename(TEMP_DB, CARDAPIO_DB) != 0) {
        printf("\033[1;31mErro ao renomear arquivo! Permissao negada.\033[0m\n");

        // Mensagem de erro com formatação
        mid_line();
        remove(TEMP_DB);
        return false;
    }

    // Define permissões explicitamente (opcional)
    chmod(CARDAPIO_DB, 0644); // rw-r--r--

    return true;
}

/* Conta itens ativos no cardápio */
int contar_itens_ativos() {
    int total;

    // Carrega cardápio do arquivo
    ItemCardapio* itens = carregar_cardapio(&total);

    // Se não houver itens ou o carregamento falhar, retorna 0
    if (!itens) return 0;

    int count = 0;

    // Conta itens ativos
    for (int i = 0; i < total; i++) {
        if (itens[i].ativo) count++;
    }

    // Libera memória alocada
    liberar_cardapio(itens);
    return count;
}

/* Libera memória alocada para o cardápio */
void liberar_cardapio(ItemCardapio* itens) {
    if (itens) free(itens);
}

/* Adiciona novo item ao cardápio */
bool adicionar_item() {

    // Verifica se o usuário tem permissão para modificar o cardápio
    if (!verificar_permissoes()) {
        pausar_tela();
        return false;
    }

    system("clear || cls");

    top_bottom();
    printf("ADICIONAR ITEM AO CARDAPIO\n");
    mid_line();

    
    ItemCardapio novo; // Estrutura para novo item
    
    // Nome do item
    // Loop até receber um nome válido
    do {
        char nome[50];
        printf("Nome do item: ");
        fgets(nome, sizeof(nome), stdin);
        nome[strcspn(nome, "\n")] = '\0';
        
        if (validar_nome_item(nome)) { // Valida o nome
            strcpy(novo.nome, nome);
            break; // Se válido, copia para a estrutura
        }
        printf("\033[1;31mNome invalido! Use apenas letras e espacos.\033[0m\n"); 
        mid_line();
 
    } while (1);

    // Tipo do item
    // Loop até receber um tipo válido
    novo.tipo = ler_inteiro_seguro("Tipo (0-Comida, 1-Bebida, 2-Sobremesa): ", 0, 2);

    // Valor do item
    // Loop até receber um valor positivo
    novo.valor = ler_float_positivo("Valor (ex: 12.50): R$");
    novo.ativo = true;

    int total;

    // Carrega cardápio existente
    // Se não houver itens, total será 0
    ItemCardapio* itens = carregar_cardapio(&total);

    if (!itens && total != 0) {
        return false;
    }

    // Aloca espaço para novo item
    ItemCardapio* novo_cardapio = (ItemCardapio*)realloc(itens, (total + 1) * sizeof(ItemCardapio));

    // Verifica se a alocação foi bem-sucedida
    if (!novo_cardapio) {
        liberar_cardapio(itens);
        printf("\033[1;31mErro ao alocar memoria!\033[0m\n");
        return false;
    }

    // Copia novo item para o final do array
    novo_cardapio[total] = novo;
    
    // Marca o item como ativo
    if (!salvar_cardapio(novo_cardapio, total + 1)) {
        liberar_cardapio(novo_cardapio);
        return false;
    }
    
    // Libera memória do cardápio antigo
    liberar_cardapio(novo_cardapio);
    printf("\033[1;32mItem adicionado com sucesso!\033[0m\n");
    mid_line();

    pausar_tela();
    system("clear || cls");
    top_bottom();

    return true; 
}

/* Edita item existente no cardápio */
bool editar_item() {
    if (!verificar_permissoes()) {
        pausar_tela();
        return false;
    }

    int total;

    // Carrega cardápio do arquivo
    ItemCardapio* itens = carregar_cardapio(&total);

    // Se não houver itens ou o carregamento falhar, retorna false
    if (!itens || total == 0) {
        printf("\033[1;33mCardapio vazio!\033[0m\n");
        mid_line();
        pausar_tela();

        system("clear || cls");
        top_bottom();
        return false;
    }

    exibir_cardapio();
    
    // Solicita posição do item a editar
    int pos = ler_inteiro_seguro("Digite a posicao do item a editar (0 para cancelar):", 0, total);

    // Se o usuário digitar 0, cancela a edição
    if (pos == 0) {
        system("clear || cls");
        top_bottom();

        liberar_cardapio(itens);
        return true;
    }
    
    pos--; // Ajuste para índice interno
    
    // Edição do item
    printf("Editando: %s\n", itens[pos].nome);
    mid_line();
    
    // Novo nome do item
    char novo_nome[50];
    printf("Novo nome: ");
    fgets(novo_nome, sizeof(novo_nome), stdin);
    novo_nome[strcspn(novo_nome, "\n")] = '\0';
    
    // Se o nome for válido, atualiza o item
    if (strlen(novo_nome) > 0 && validar_nome_item(novo_nome)) {
        strcpy(itens[pos].nome, novo_nome);
    }
    
    // Novo tipo do item
    itens[pos].tipo = ler_inteiro_seguro("Novo tipo (0-Comida, 1-Bebida, 2-Sobremesa): ", 0, 2);
    
    // Novo valor do item
    itens[pos].valor = ler_float_positivo("Novo valor: R$"); 


    bool sucesso = salvar_cardapio(itens, total); // Salva as alterações no cardápio
    liberar_cardapio(itens); // Libera memória alocada
     
    // Se a operação foi bem-sucedida, exibe mensagem de sucesso
    if (sucesso) {
        printf("\033[1;32mItem atualizado com sucesso!\033[0m\n");
        mid_line();

        pausar_tela();
        system("clear || cls");
        top_bottom();
         
        return true;
    }
    return false;
}

/* Remove item do cardápio (deleção lógica) */
bool excluir_item() {
    if (!verificar_permissoes()) {
        pausar_tela();
        return false;
    }

    int total;

    // Carrega cardápio do arquivo
    ItemCardapio* itens = carregar_cardapio(&total);

    // Se não houver itens ou o carregamento falhar, retorna false
    if (!itens || total == 0) {
        printf("\033[1;33mCardapio vazio!\033[0m\n");
        mid_line();
        pausar_tela();

        system("clear || cls");
        top_bottom();
        return false;
    }

    exibir_cardapio();
    
    // Solicita posição do item a remover
    int pos = ler_inteiro_seguro("Digite a posicao do item a remover (0 para cancelar):", 0, total);

    // Se o usuário digitar 0, cancela a remoção
    if (pos == 0) {
        liberar_cardapio(itens);
        return true;
    }
    
    pos--; // Ajuste para índice interno
    
    bool sucesso = false;

    mid_line();

    // Pergunta de confirmação
    if (validar_resposta_sn("Tem certeza que deseja remover este item?")) { 
        itens[pos].ativo = false;
        sucesso = salvar_cardapio(itens, total);
    }
    
    // Libera memória alocada
    liberar_cardapio(itens);
    
    // Se a operação foi bem-sucedida, exibe mensagem de sucesso
    if (sucesso) {
        printf("\033[1;32mItem removido com sucesso!\033[0m\n");
        mid_line();

        pausar_tela();
        system("clear || cls");
        top_bottom();

        return true;
    }
    
    // Se a operação não foi bem-sucedida, exibe mensagem de erro
    printf("\033[1;33mOperacao cancelada!\033[0m\n");

    mid_line();
    pausar_tela();
    return false;
}

/* Exibe todos os itens ativos do cardápio */
void exibir_cardapio() {
    int total;

    // Carrega cardápio do arquivo
    ItemCardapio* itens = carregar_cardapio(&total);

    // Se não houver itens ou o carregamento falhar, exibe mensagem e retorna
    if (!itens || total == 0) {
        printf("\033[1;33mCardapio vazio!\033[0m\n");
        mid_line();
        pausar_tela();

        system("clear || cls");
        top_bottom(); 
        return;
    }

    system("clear || cls");
    top_bottom();

    printf("CARDAPIO COMPLETO\n");
    mid_line();
    
    int ativos = 0; // Contador de itens ativos

    // Exibe cabeçalho da tabela
    for (int i = 0; i < total; i++) {

        // Exibe apenas itens ativos
        if (itens[i].ativo) {
            printf("%d. %-25s | %-9s | R$ %7.2f\n", 
                  i+1,
                  itens[i].nome,
                  itens[i].tipo == COMIDA ? "Comida" : 
                  itens[i].tipo == BEBIDA ? "Bebida" : "Sobremesa",
                  itens[i].valor);
            ativos++;
        }
    }
    
    // Se não houver itens ativos, exibe mensagem
    if (ativos == 0) {
        printf("\033[1;33mNenhum item ativo no cardapio!\033[0m\n");
    }
    

    mid_line();
    printf("Total de itens ativos: %d\n", ativos);

    top_bottom();
    pausar_tela();
    mid_line();

    liberar_cardapio(itens); // Libera memória alocada
}