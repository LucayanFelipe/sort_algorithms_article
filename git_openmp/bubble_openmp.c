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

void oddEvenSortParalelo(int *v, int n) {
    for (int i = 0; i < n; i++) {
        int start = i % 2;
        #pragma omp parallel for
        for (int j = start; j < n - 1; j += 2) {
            if (v[j] > v[j + 1]) {
                int temp = v[j];
                v[j] = v[j + 1];
                v[j + 1] = temp;
            }
        }
    }
}

int main() {
    srand(time(NULL));
    int tam = 1000000; 
    double tempo_total = 0;

    omp_set_num_threads(omp_get_max_threads());

    for (int i = 0; i < 100; i++) {
        printf("\nDESORDENADO #%d tamanho (%d)\n", i + 1, tam);
        int *v = gerarArray(tam);
        printarArrayLimitado(v, tam);

        double inicio = omp_get_wtime();
        oddEvenSortParalelo(v, tam);
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
