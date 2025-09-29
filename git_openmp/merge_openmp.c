#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

int* gerarArray(int tam) {
    int *v = (int*)malloc(tam * sizeof(int));
    if (v == NULL) {
        fprintf(stderr, "Erro ao alocar memoria.\n");
        exit(1);
    }
    for (int i = 0; i < tam; i++) {
        v[i] = rand() % 100;
    }
    return v;
}

void printarArrayLimitado(int *v, int tam) {
    printf("\nArray (limitado): \n");
    int limite = tam < 100 ? tam : 50;

    for (int i = 0; i < limite; i++) {
        printf("%d ", v[i]);
    }

    if (tam > 100) {
        printf("... ");
        for (int i = tam - limite; i < tam; i++) {
            printf("%d ", v[i]);
        }
    }
    printf("\n");
}

void merge(int *v, int l, int m, int r) {
    int n1 = m - l + 1;
    int n2 = r - m;
    int *L = (int*)malloc(n1 * sizeof(int));
    int *R = (int*)malloc(n2 * sizeof(int));

    for (int i = 0; i < n1; i++) L[i] = v[l + i];
    for (int j = 0; j < n2; j++) R[j] = v[m + 1 + j];

    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2) {
        v[k++] = (L[i] <= R[j]) ? L[i++] : R[j++];
    }
    while (i < n1) v[k++] = L[i++];
    while (j < n2) v[k++] = R[j++];

    free(L);
    free(R);
}

void merge_recursionParallel(int *v, int l, int r, int profundidade) {
    if (l < r) {
        int m = l + (r - l) / 2;

        if (profundidade <= 4) { 
            #pragma omp task shared(v)
            merge_recursionParallel(v, l, m, profundidade + 1);

            #pragma omp task shared(v)
            merge_recursionParallel(v, m + 1, r, profundidade + 1);

            #pragma omp taskwait
        } else {
            merge_recursionParallel(v, l, m, profundidade + 1);
            merge_recursionParallel(v, m + 1, r, profundidade + 1);
        }

        merge(v, l, m, r);
    }
}

void mergeSortParalelo(int *v, int tam) {
    #pragma omp parallel
    {
        #pragma omp single nowait
        {
            printf("Usando %d threads\n", omp_get_num_threads());
            merge_recursionParallel(v, 0, tam - 1, 0);
        }
    }
}

int main() {
    srand(time(NULL));
    int tam = 100000000;
    double tempo_total=0;

    for (int i = 0; i < 100; i++) {
        printf("\nDESORDENADO #%d tamanho (%d)", i + 1, tam);

        int *v = gerarArray(tam);
        printarArrayLimitado(v, tam);
        omp_set_num_threads(omp_get_max_threads());

        double inicio = omp_get_wtime();
        mergeSortParalelo(v, tam);
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

