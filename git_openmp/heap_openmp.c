#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

int* gerarArray(int tam) {
    int *v;
    v = (int*)malloc(tam * sizeof(int));
    #pragma omp parallel for 
    for (int i = 0; i < tam; i++) {
        v[i] = rand() % 100;
    }
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

void heapar(int *v, int tam, int i) {
    int pai = i;
    int l = 2 * i + 1;
    int r = 2 * i + 2;

    if (l < tam && v[l] > v[pai]) {
        pai = l;
    }
    if (r < tam && v[r] > v[pai]) {
        pai = r;
    }

    if (pai != i) {
        int troca = v[i];
        v[i] = v[pai];
        v[pai] = troca;
        heapar(v, tam, pai);
    }
}


void heapSortProcessParallel(int *v, int tam) {

    #pragma omp parallel for schedule(dynamic)
    for (int i = tam / 2 - 1; i >= 0; i--) {
        heapar(v, tam, i);
    }


    for (int i = tam - 1; i > 0; i--) {
        int troca = v[0];
        v[0] = v[i];
        v[i] = troca;

        heapar(v, i, 0); 
    }
}

void heapSortParalelo(int *v, int tam) {
    heapSortProcessParallel(v, tam);
}

int main() {
    srand(time(NULL));
    int tam = 100000000;
    double tempo_total = 0;

    for (int i = 0; i < 100; i++) {
        printf("\nDESORDENADO #%d tamanho (%d)", i + 1, tam);

        int *v = gerarArray(tam);
        printarArrayLimitado(v, tam);

        omp_set_num_threads(omp_get_max_threads());

        double inicio = omp_get_wtime();
        heapSortParalelo(v, tam);
        double fim = omp_get_wtime();

        printf("\nORDENADO #%d tamanho (%d)", i + 1, tam);
        printarArrayLimitado(v, tam);
        printf("Tempo de execucao: %.3f segundos\n", fim - inicio);
        tempo_total += (fim - inicio);

        free(v);
    }

    printf("\nTempo medio por iteracao: %.6f segundos\n", tempo_total / 100.0);
    return 0;
}
