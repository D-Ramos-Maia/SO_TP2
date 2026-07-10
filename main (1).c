#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <pthread.h>

#ifdef _WIN32
    #include <windows.h>
#elif defined(__linux__) || defined(__APPLE__)
    #include <unistd.h>
#endif

typedef struct {
    int id;
    char nome_arquivo[100];
    int q_elementos; 
    int limite_maximo; 
    int resultado;
} dados;

// Retorna o número de cores lógicos do sistema
long n_threads() {
    #ifdef _WIN32
        SYSTEM_INFO sysinfo;
        GetSystemInfo(&sysinfo);
        return sysinfo.dwNumberOfProcessors;
    #elif defined(__linux__) || defined(__APPLE__)
        long threads = sysconf(_SC_NPROCESSORS_ONLN);
        if (threads == -1) return 1;
        return threads;
    #else
        return 1;
    #endif
}

// FUNÇÃO DE ALTA PRECISÃO PARA TEMPO DE RELÓGIO (WALL-CLOCK)
double obter_tempo_atual() {
    #ifdef _WIN32
        LARGE_INTEGER frequencia, contador;
        QueryPerformanceFrequency(&frequencia);
        QueryPerformanceCounter(&contador);
        return (double)contador.QuadPart / frequencia.QuadPart;
    #elif defined(__linux__) || defined(__APPLE__)
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        return ts.tv_sec + ts.tv_nsec * 1e-9;
    #else
        return (double)time(NULL);
    #endif
}

void criar_arquivos(long n_arquivo) {
    char nome_arquivo[100];
    for (int i = 0; i < n_arquivo; i++) {
        sprintf(nome_arquivo, "%02d.txt", i);
        FILE *arquivo = fopen(nome_arquivo, "w");
        if (arquivo != NULL) {
            fclose(arquivo);
        }
    }
}

void encher_arquivo(char* nome_arquivo, int q_elementos) {
    FILE *arquivo = fopen(nome_arquivo, "w");
    if (arquivo == NULL) return;

    for (int i = 0; i < q_elementos; i++) {
        int valor = (rand() % q_elementos) + 1; 
        fprintf(arquivo, "%d\n", valor);
    }
    fclose(arquivo);
}

int ache_elemento(char* nome_arquivo, int q_elemento) {
    FILE *arquivo = fopen(nome_arquivo, "r");
    if (arquivo == NULL) return -1;

    char linha[100];
    int quanti = 0;

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        size_t len = strlen(linha);
        if (len > 0 && linha[len-1] == '\n') {
            linha[len-1] = '\0';
        }

        int control = atoi(linha);
        if (control == q_elemento) {
            quanti++;
        }
    }

    fclose(arquivo);
    return quanti;
}

void* thread_busca(void* arg) {
    dados* d = (dados*) arg;
    
    printf("[Thread %d] Iniciou busca no arquivo %s...\n", d->id, d->nome_arquivo);
    
    if (d->q_elementos < 1 || d->q_elementos > d->limite_maximo) {
        d->resultado = -2; 
    } else {
        d->resultado = ache_elemento(d->nome_arquivo, d->q_elementos);
    }
    
    printf("[Thread %d] Concluiu busca!\n", d->id);
    pthread_exit(NULL);
}

int main(void) {
    srand(time(NULL)); 

    long total_arquivos = n_threads();
    printf("Configuracao de Hardware: %ld cores detectados.\n", total_arquivos);
    
    // Permitir ao usuário escolher a quantidade de threads (Requisito da Q2)
    int q_threads;
    printf("Quantas threads/arquivos deseja utilizar para o teste? ");
    if (scanf("%d", &q_threads) != 1 || q_threads <= 0 || q_threads > total_arquivos) {
        q_threads = (int)total_arquivos;
        printf("Entrada invalida. Usando padrao do sistema: %d\n", q_threads);
    }

    criar_arquivos(q_threads);
    int limites_arquivos[q_threads]; 

    printf("\nGerando arquivos de teste com tamanhos grandes para dar carga de processamento...\n");
    for (int i = 0; i < q_threads; i++) {
        char nome_arquivo[100];
        sprintf(nome_arquivo, "%02d.txt", i);
        
        // Usando um valor alto fixo para viabilizar testes de performance reprodutíveis
        limites_arquivos[i] = 1000000; 
        encher_arquivo(nome_arquivo, limites_arquivos[i]);
    }
    printf("Todos os arquivos foram preenchidos com 1.000.000 de elementos cada.\n");

    int numero_para_buscar;
    printf("\nNumero para busca (de 1 a 1000000): ");
    scanf("%d", &numero_para_buscar);

    // ==========================================
    // 1. EXECUÇÃO SEQUENCIAL TRADICIONAL (Ts)
    // ==========================================
    printf("\n--- INICIANDO BUSCA SEQUENCIAL ---\n");
    double tempo_inicio_seq = obter_tempo_atual();
    
    int total_encontrado_seq = 0;
    for (int i = 0; i < q_threads; i++) {
        char nome_arquivo[100];
        sprintf(nome_arquivo, "%02d.txt", i);
        int res = ache_elemento(nome_arquivo, numero_para_buscar);
        if (res > 0) total_encontrado_seq += res;
    }
    
    double tempo_fim_seq = obter_tempo_atual();
    double Ts = tempo_fim_seq - tempo_inicio_seq;
    printf("Busca Sequencial concluida em: %.6f segundos.\n", Ts);


    // ==========================================
    // 2. EXECUÇÃO PARALELA COM THREADS (Tp)
    // ==========================================
    printf("\n--- INICIANDO BUSCA PARALELA ---\n");
    double tempo_inicio_par = obter_tempo_atual();

    pthread_t tt[q_threads];
    dados contratos[q_threads]; 

    for (int i = 0; i < q_threads; i++) {
        contratos[i].id = i;
        sprintf(contratos[i].nome_arquivo, "%02d.txt", i);
        contratos[i].q_elementos = numero_para_buscar; 
        contratos[i].limite_maximo = limites_arquivos[i]; 
        contratos[i].resultado = 0;

        pthread_create(&tt[i], NULL, thread_busca, (void*)&contratos[i]);
    }

    int total_encontrado_par = 0;
    for (int i = 0; i < q_threads; i++) {
        pthread_join(tt[i], NULL);
        if (contratos[i].resultado > 0) {
            total_encontrado_par += contratos[i].resultado;
        }
    }

    double tempo_fim_par = obter_tempo_atual();
    double Tp = tempo_fim_par - tempo_inicio_par;
    printf("Busca Paralela concluida em: %.6f segundos.\n", Tp);


    // ==========================================
    // 3. CÁLCULO DE DESEMPENHO (SPEEDUP)
    // ==========================================
    double speedup = Ts / Tp;

    printf("\n=========================================\n");
    printf("              RELATORIO FINAL            \n");
    printf("=========================================\n");
    printf("Total Encontrado (Seq): %d\n", total_encontrado_seq);
    printf("Total Encontrado (Par): %d\n", total_encontrado_par);
    printf("Tempo Sequencial (Ts):  %.6f seg\n", Ts);
    printf("Tempo Paralelo (Tp):    %.6f seg\n", Tp);
    printf("Metrica de Aceleracao / Speedup (Sp): %.2fx\n", speedup);
    printf("=========================================\n");

    if (speedup > 1.0) {
        printf("Ganho de desempenho: A versao paralela foi %.2f vezes mais rapida!\n", speedup);
    } else {
        printf("Atencao: A sobrecarga (overhead) de criação das threads superou o ganho paralelo.\n");
    }

    return 0;
}