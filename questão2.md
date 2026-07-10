# Instruções de Compilação e Execução - Questão 2
**Trabalho Prático 2 - Sistemas Operacionais**

Este diretório contém a implementação do **Sistema Interativo de Busca em Arquivos**, utilizando concorrência através da biblioteca POSIX Threads (`pthread.h`), além de um script de automação em Python para a coleta de métricas de desempenho (*Benchmark*) e geração de gráficos.

---

## 1. Pré-requisitos do Sistema
Para executar este projeto corretamente, certifique-se de ter os seguintes softwares instalados no seu ambiente:
* **Compilador C (GCC / MinGW):** Necessário para compilar o código fonte `.c`.
* **Python 3.x:** Necessário para rodar a automação do benchmark.
* **Gerenciador de Pacotes Python (pip):** Para instalar as bibliotecas de plotagem e dados.

## 2. Como compilar o programa em C
O código principal em C depende da biblioteca `pthread`. Siga os passos abaixo no seu terminal (PowerShell, CMD ou terminal do Linux/macOS):

1. Navegue até a pasta onde o arquivo `.c` está localizado.
2. Execute o seguinte comando de compilação:
   ```bash
   gcc main.c -o busca_q2.exe 
3. Execute o seguinte comando para rodar o programa:
   ```bash
   .\busca_q2.exe   

## 2. Como compilar o programa de benchmark em python
O código para geração de dados e graficos em python depende da biblioteca `matplotlib`. Siga os passos abaixo no seu terminal (PowerShell, CMD ou terminal do Linux/macOS):

1. Navegue até a pasta onde o arquivo `.python` está localizado.
2. Execute o seguinte comando de compilação:
   ```bash
    python .\benchmark.py  