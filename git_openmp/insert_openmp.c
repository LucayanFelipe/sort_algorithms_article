#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

int* gerarArray(int tam) {
    int *v = (int*)malloc(tam * sizeof(int));
    for (int i = 0; i < tam; i++) {
        v[i] = rand() % 100;
    }
    return v;
}

void printarArrayLimitado(int *v, long long tam) {
    if (tam <= 100) {
        for (long long i = 0; i < tam; i++) printf("%d ", v[i]);
        printf("\n");
    } else {
        for (long long i = 0; i < 50; i++) printf("%d ", v[i]);
        printf("... ");
        for (long long i = tam - 50; i < tam; i++) printf("%d ", v[i]);
        printf(" (total %lld elementos)\n", tam);
    }
}

void insertSort(int *v, long long left, long long right) {
    for (long long i = left + 1; i <= right; i++) {
        int temp = v[i];
        long long j = i - 1;
        while (j >= left && v[j] > temp) {
            v[j + 1] = v[j];
            j--;
        }
        v[j + 1] = temp;
    }
}

void mergeBlocos(int *v, int tam, int num_blocos) {
    int *indices = (int*)malloc(num_blocos * sizeof(int));
    int bloco_tam = tam / num_blocos;
    int resto = tam % num_blocos;

    int *saida = (int*)malloc(tam * sizeof(int));

    for (int i = 0; i < num_blocos; i++) indices[i] = 0;

    for (long long k = 0; k < tam; k++) {
        int menor = 1e9;
        int bloco_menor = -1;
        for (int i = 0; i < num_blocos; i++) {
            long long start = i * bloco_tam + (i < resto ? i : resto);
            long long end = start + bloco_tam - 1 + (i < resto ? 1 : 0);
            if (indices[i] <= end - start) {
                int val = v[start + indices[i]];
                if (val < menor) {
                    menor = val;
                    bloco_menor = i;
                }
            }
        }
        saida[k] = menor;
        indices[bloco_menor]++;
    }

    for (long long i = 0; i < tam; i++) v[i] = saida[i];

    free(indices);
    free(saida);
}


void insertSortDistribuido(int *v, long long tam) {
    int num_threads = omp_get_max_threads();
    long long bloco_tam = tam / num_threads;
    long long resto = tam % num_threads;

    #pragma omp parallel num_threads(num_threads)
    {
        int id = omp_get_thread_num();
        long long left = id * bloco_tam + (id < resto ? id : resto);
        long long right = left + bloco_tam - 1 + (id < resto ? 1 : 0);
        insertSort(v, left, right); 
    }


    mergeBlocos(v, tam, num_threads);
}

int main() {
    srand(time(NULL));
    long long tam = 1000000; // 1 milhão
    double tempo_total = 0;

    omp_set_num_threads(omp_get_max_threads());

    for (int i = 0; i < 100; i++) {
        printf("\nDESORDENADO #%d tamanho (%lld)\n", i + 1, tam);

        int *v = gerarArray(tam);
        printarArrayLimitado(v, tam);

        double inicio = omp_get_wtime();
        insertSortDistribuido(v, tam);
        double fim = omp_get_wtime();

        printf("\nORDENADO #%d tamanho (%lld)\n", i + 1, tam);
        printarArrayLimitado(v, tam);
        printf("Tempo de execucao: %.6f segundos\n", fim - inicio);
        tempo_total += (fim - inicio);

        free(v);
    }

    printf("\nTempo medio por iteracao: %.6f segundos\n", tempo_total / 100.0);
    return 0;
}
