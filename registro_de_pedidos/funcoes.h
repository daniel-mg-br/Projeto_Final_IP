#pragma once

#define ARQUIVO_BINARIO "pedidos.bin"
#define MAX_CUPONS 5

// Estrutura que representa um cupom de desconto
struct cupom_desconto {
    char codigo[20];
    float porcentagem;
};

// Estrutura que representa um pedido
struct dados_pedido {
    int mesa;
    int pessoas;
    char item[50];
    int tipo; // 1 = comida, 2 = bebida
    int quantidade;
    float subtotal;
    float valor_individual;
    char cupom[20];
    float desconto; // de 0.0 a 1.0
};

// Funções principais do sistema
void registrar_e_salvar(struct dados_pedido*, int*, int, int, char[], int, int, const char*, float);
int carregar_pedidos(struct dados_pedido vetor[], int *total_pedidos);
void exibir_pedido(struct dados_pedido p, int eh_relatorio);
void exibir_resumo(struct dados_pedido pedidos[], int num_pedidos);
void gerar_relatorio_final();
void calcular_metricas_gerais(struct dados_pedido pedidos[], int total_pedidos);
void calcular_subtotal_por_tipo(struct dados_pedido pedidos[], int total_pedidos);
void gerar_ranking(struct dados_pedido pedidos[], int total_pedidos);

// Cupons
void inicializar_cupons(struct cupom_desconto cupons[]);
int aplicar_cupom(char codigo[], struct cupom_desconto cupons[], float *desconto);

// Estética
void mid_line();
void top_bottom();

// Validação
int validar_resposta_sn(const char *mensagem);
int ler_inteiro_seguro(const char *mensagem, int min, int max);
int validar_nome_item(const char *str);