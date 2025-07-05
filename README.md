Descrição

O objetivo deste projeto é desenvolver um sistema em linguagem C para gerenciar pedidos de um restaurante. O sistema permitirá o cadastro de pratos do menu, o registro dos pedidos feitos pelos clientes, o cálculo do valor total da conta e a geração de um relatório diário de vendas. Todas as informações de pedidos deverão ser armazenadas em um arquivo binário para garantir a persistência dos dados.

Funcionalidades Requeridas:

    1. Cadastro de Pratos do Menu

        ○ O sistema deve permitir o cadastro de pratos disponíveis no cardápio, registrando:
            ■ Nome do prato
            ■ Preço unitário

    2. Registro de Pedidos

        ○ O sistema deve permitir que os clientes façam pedidos selecionando pratos do menu.

        ○ Cada pedido deve conter:
            ■ Nome do prato escolhido
            ■ Quantidade pedida
            ■ Valor total do pedido (quantidade × preço do prato)

        ○ O sistema deve permitir que os pedidos sejam associados a uma mesa específica.

    3. Cálculo do Total da Conta

        ○ O sistema deve calcular automaticamente o valor total da conta de um cliente somando os valores dos pedidos feitos.

        ○ O sistema deve realizar a divisão da conta por cliente sentado à mesa.

    4. Geração de Relatório Diário de Vendas

        ○ O sistema deve gerar um relatório ao final do dia contendo:
            ■ Lista de todos os pedidos realizados.
            ■ Total arrecadado no dia.
            ■ Pratos mais vendidos.

        ○ Criar uma opção para aplicar descontos e cupons promocionais.

    5. Persistência de Dados

        ○ Todos os pedidos devem ser armazenados em um arquivo binário, garantindo que as informações sejam preservadas após o encerramento do programa.

        ○ O sistema deve carregar os dados do arquivo binário ao ser iniciado, permitindo a consulta de pedidos anteriores.

Requisitos Técnicos:
● O sistema deve ser implementado em C.
● O uso de structs, vetores, arquivos binários e ponteiros é obrigatório.
● O código deve ser modularizado, com funções bem definidas para cada operação do sistema.
● A interação com o usuário deve ser feita por entrada e saída de texto no console.