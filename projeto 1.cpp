
#include <stdio.h>
#include <string.h>

#define MAX_PROCESSOS 50

typedef struct {
    char nome[30];
    int chegada;
    int burst;
    int restante;
    int concluido;
    int inicio;
    int termino;
    int jaEntrou;
} Processo;

/* Limpa o restante da linha digitada. */
void limparBuffer(void) {
    int c;

    while ((c = getchar()) != '\n' && c != EOF) {
    }
}

/* Recebe e prepara os processos para a simulacao. */
void cadastrarProcessos(Processo p[], int *n) {
    int i;

    do {
        printf("\nQuantidade de processos (1 a %d): ",
               MAX_PROCESSOS);

        if (scanf("%d", n) != 1) {
            printf("Digite um numero valido.\n");
            limparBuffer();
            *n = 0;
            continue;
        }

        limparBuffer();

        if (*n < 1 || *n > MAX_PROCESSOS) {
            printf("Quantidade invalida.\n");
        }

    } while (*n < 1 || *n > MAX_PROCESSOS);

    for (i = 0; i < *n; i++) {
        printf("\nCadastro do processo %d\n", i + 1);

        printf("Nome: ");
        scanf("%29s", p[i].nome);
        limparBuffer();

        do {
            printf("Tempo de chegada: ");

            if (scanf("%d", &p[i].chegada) != 1) {
                printf("Digite um numero inteiro.\n");
                limparBuffer();
                p[i].chegada = -1;
                continue;
            }

            limparBuffer();

            if (p[i].chegada < 0) {
                printf("O tempo nao pode ser negativo.\n");
            }

        } while (p[i].chegada < 0);

        do {
            printf("Tempo de CPU: ");

            if (scanf("%d", &p[i].burst) != 1) {
                printf("Digite um numero inteiro.\n");
                limparBuffer();
                p[i].burst = 0;
                continue;
            }

            limparBuffer();

            if (p[i].burst <= 0) {
                printf("O tempo deve ser maior que zero.\n");
            }

        } while (p[i].burst <= 0);

        p[i].restante = p[i].burst;
        p[i].concluido = 0;
        p[i].inicio = -1;
        p[i].termino = -1;
        p[i].jaEntrou = 0;
    }
}

/* Retorna 1 quando todos os processos terminaram. */
int todosConcluidos(Processo p[], int n) {
    int i;

    for (i = 0; i < n; i++) {
        if (!p[i].concluido) {
            return 0;
        }
    }

    return 1;
}

/* Escolhe o processo com menor tempo restante. */
int escolherSRT(Processo p[], int n, int tempo) {
    int i;
    int escolhido = -1;

    for (i = 0; i < n; i++) {
        if (p[i].chegada <= tempo &&
            p[i].restante > 0 &&
            !p[i].concluido) {

            if (escolhido == -1 ||
                p[i].restante < p[escolhido].restante) {

                escolhido = i;
            }
        }
    }

    return escolhido;
}

/* Mostra a CPU e os processos que estao esperando. */
void mostrarEstadoSRT(Processo p[], int n,
                      int tempo, int cpu) {
    int i;
    int encontrou = 0;

    printf("\n----------------------------------\n");
    printf("Tempo atual: %d\n", tempo);

    if (cpu == -1) {
        printf("CPU: OCIOSA\n");
    } else {
        printf("CPU: %s (restante: %d)\n",
               p[cpu].nome, p[cpu].restante);
    }

    printf("Fila de prontos:\n");

    for (i = 0; i < n; i++) {
        if (i != cpu &&
            p[i].chegada <= tempo &&
            p[i].restante > 0 &&
            !p[i].concluido) {

            printf("%s - restante: %d\n",
                   p[i].nome, p[i].restante);

            encontrou = 1;
        }
    }

    if (!encontrou) {
        printf("Nenhum processo aguardando.\n");
    }
}

/* Executa a simulacao do algoritmo SRT. */
void executarSRT(Processo p[], int n) {
    int tempo = 0;
    int cpu;
    int i;

    printf("\n======= ALGORITMO SRT =======\n");

    while (!todosConcluidos(p, n)) {
        cpu = escolherSRT(p, n, tempo);

        mostrarEstadoSRT(p, n, tempo, cpu);

        if (cpu == -1) {
            printf("Intervalo [%d, %d): CPU ociosa\n",
                   tempo, tempo + 1);

            tempo++;
            continue;
        }

        if (p[cpu].inicio == -1) {
            p[cpu].inicio = tempo;
        }

        printf("Executando %s no intervalo [%d, %d)\n",
               p[cpu].nome, tempo, tempo + 1);

        p[cpu].restante--;
        tempo++;

        if (p[cpu].restante == 0) {
            p[cpu].concluido = 1;
            p[cpu].termino = tempo;

            printf("%s terminou no tempo %d.\n",
                   p[cpu].nome, tempo);
        }
    }

    printf("\n======= RESULTADO SRT =======\n");

    for (i = 0; i < n; i++) {
        printf("%s | Chegada: %d | CPU: %d | "
               "Inicio: %d | Termino: %d\n",
               p[i].nome, p[i].chegada,
               p[i].burst, p[i].inicio,
               p[i].termino);
    }
}

/* Adiciona um processo ao final da fila circular. */
void enfileirar(int fila[], int *fim, int *quantidade,
                int processo) {
    fila[*fim] = processo;
    *fim = (*fim + 1) % MAX_PROCESSOS;
    (*quantidade)++;
}

/* Retira o primeiro processo da fila. */
int desenfileirar(int fila[], int *inicio,
                  int *quantidade) {
    int processo = fila[*inicio];

    *inicio = (*inicio + 1) % MAX_PROCESSOS;
    (*quantidade)--;

    return processo;
}

/* Exibe a fila e o tempo restante de cada processo. */
void mostrarEstadoRR(Processo p[], int fila[],
                     int inicio, int quantidade,
                     int cpu, int tempo) {
    int i;
    int pos;

    printf("\n----------------------------------\n");
    printf("Tempo atual: %d\n", tempo);

    if (cpu == -1) {
        printf("CPU: OCIOSA\n");
    } else {
        printf("CPU: %s (restante: %d)\n",
               p[cpu].nome, p[cpu].restante);
    }

    printf("Fila de prontos: ");

    if (quantidade == 0) {
        printf("vazia");
    } else {
        for (i = 0; i < quantidade; i++) {
            pos = (inicio + i) % MAX_PROCESSOS;

            printf("%s (restante: %d)",
                   p[fila[pos]].nome,
                   p[fila[pos]].restante);

            if (i < quantidade - 1) {
                printf(" -> ");
            }
        }
    }

    printf("\n");
}

/* Executa a simulacao do algoritmo Round-Robin. */
void executarRoundRobin(Processo p[], int n, int quantum) {
    int fila[MAX_PROCESSOS];
    int inicioFila = 0;
    int fimFila = 0;
    int quantidade = 0;
    int tempo = 0;
    int cpu = -1;
    int fatia = 0;
    int i;

    printf("\n======= ALGORITMO ROUND-ROBIN =======\n");
    printf("Quantum: %d\n", quantum);

    while (!todosConcluidos(p, n)) {

        /* Coloca cada processo na fila apenas uma vez. */
        for (i = 0; i < n; i++) {
            if (!p[i].jaEntrou &&
                p[i].chegada <= tempo) {

                enfileirar(fila, &fimFila,
                           &quantidade, i);

                p[i].jaEntrou = 1;
            }
        }

        /* Seleciona o proximo processo, se necessario. */
        if (cpu == -1 && quantidade > 0) {
            cpu = desenfileirar(fila, &inicioFila,
                                &quantidade);

            fatia = 0;

            if (p[cpu].inicio == -1) {
                p[cpu].inicio = tempo;
            }
        }

        mostrarEstadoRR(p, fila, inicioFila,
                        quantidade, cpu, tempo);

        if (cpu == -1) {
            printf("Intervalo [%d, %d): CPU ociosa\n",
                   tempo, tempo + 1);

            tempo++;
            continue;
        }

        printf("Executando %s no intervalo [%d, %d)\n",
               p[cpu].nome, tempo, tempo + 1);

        p[cpu].restante--;
        fatia++;
        tempo++;

        if (p[cpu].restante == 0) {
            p[cpu].concluido = 1;
            p[cpu].termino = tempo;

            printf("%s terminou no tempo %d.\n",
                   p[cpu].nome, tempo);

            cpu = -1;
            fatia = 0;

        } else if (fatia == quantum) {

            /*
             * Os processos que chegaram neste instante
             * entram antes do processo que perdeu a vez.
             */
            for (i = 0; i < n; i++) {
                if (!p[i].jaEntrou &&
                    p[i].chegada <= tempo) {

                    enfileirar(fila, &fimFila,
                               &quantidade, i);

                    p[i].jaEntrou = 1;
                }
            }

            /* O processo volta ao final da fila. */
            enfileirar(fila, &fimFila,
                       &quantidade, cpu);

            cpu = -1;
            fatia = 0;
        }
    }

    printf("\n======= RESULTADO ROUND-ROBIN =======\n");

    for (i = 0; i < n; i++) {
        printf("%s | Chegada: %d | CPU: %d | "
               "Inicio: %d | Termino: %d\n",
               p[i].nome, p[i].chegada,
               p[i].burst, p[i].inicio,
               p[i].termino);
    }
}

/* Menu principal do programa. */
int main(void) {
    Processo processos[MAX_PROCESSOS];
    int opcao;
    int n;
    int quantum;
    int i;

    do {
        printf("\n====================================\n");
        printf(" SISTEMA DE ESCALONAMENTO DE PROCESSOS\n");
        printf("====================================\n");
        printf("1 - Simular SRT\n");
        printf("2 - Simular Round-Robin\n");
        printf("0 - Sair\n");
        printf("Escolha uma opcao: ");

        if (scanf("%d", &opcao) != 1) {
            printf("Opcao invalida.\n");
            limparBuffer();
            opcao = -1;
            continue;
        }

        limparBuffer();

        if (opcao == 1 || opcao == 2) {
            cadastrarProcessos(processos, &n);

            if (opcao == 1) {
                executarSRT(processos, n);

            } else {
                do {
                    printf("Informe o quantum: ");

                    if (scanf("%d", &quantum) != 1) {
                        printf("Digite um numero inteiro.\n");
                        limparBuffer();
                        quantum = 0;
                        continue;
                    }

                    limparBuffer();

                    if (quantum <= 0) {
                        printf("O quantum deve ser maior que zero.\n");
                    }

                } while (quantum <= 0);

                executarRoundRobin(processos, n, quantum);
            }

        } else if (opcao != 0) {
            printf("Opcao invalida. Tente novamente.\n");
        }

    } while (opcao != 0);

    printf("Programa encerrado.\n");

    return 0;
}

