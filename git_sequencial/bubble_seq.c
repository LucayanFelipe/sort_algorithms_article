#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

int* gerarArray(int tam) {
    int *v = (int*)malloc(tam * sizeof(int));
    for (int i = 0; i < tam; i++) v[i] = rand() % 100;
    return v;
}

void printarArrayLimitado(int *v, int tam) {
    if (tam <= 100) {
        for (int i = 0; i < tam; i++) printf("%d ", v[i]);
        printf("\n");
    } else {
        for (int i = 0; i < 50; i++) printf("%d ", v[i]);
        printf("... ");
        for (int i = tam - 50; i < tam; i++) printf("%d ", v[i]);
        printf(" (total %d elementos)\n", tam);
    }
}

void oddEvenSortSequencial(int *v, int n) {
    int trocado = 1;
    while (trocado) {
        trocado = 0;

        for (int i = 1; i < n - 1; i += 2) {
            if (v[i] > v[i + 1]) {
                int temp = v[i];
                v[i] = v[i + 1];
                v[i + 1] = temp;
                trocado = 1;
            }
        }

        for (int i = 0; i < n - 1; i += 2) {
            if (v[i] > v[i + 1]) {
                int temp = v[i];
                v[i] = v[i + 1];
                v[i + 1] = temp;
                trocado = 1;
            }
        }
    }
}

int main() {
    srand(time(NULL));
    int tam = 1000000;  
    double tempo_total = 0;

    for (int i = 0; i < 100; i++) {
        printf("\nDESORDENADO #%d tamanho (%d)\n", i + 1, tam);
        int *v = gerarArray(tam);
        printarArrayLimitado(v, tam);

        double inicio = omp_get_wtime();
        oddEvenSortSequencial(v, tam);
        double fim = omp_get_wtime();

        printf("\nORDENADO #%d tamanho (%d)\n", i + 1, tam);
        printarArrayLimitado(v, tam);
        printf("Tempo de execucao: %.3f segundos\n", fim - inicio);
        tempo_total += (fim - inicio);

        free(v);
    }

    printf("\nTempo medio por iteracao: %.6f segundos\n", tempo_total / 100.0);
    return 0;
}
