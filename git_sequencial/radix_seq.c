#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

int* gerarArray(int tam) {
    int *v = (int*)malloc(tam * sizeof(int));
    for (int i = 0; i < tam; i++) {
        v[i] = rand() % 100;  // valores entre 0 e 99
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

int getMaxElementArray(int *v, int tam) {
    int max = v[0];
    for (int i = 1; i < tam; i++) {
        if (v[i] > max) max = v[i];
    }
    return max;
}

void countingSort(int *v, int tam) {
    int *saida = (int*)malloc(tam * sizeof(int));
    int cont[100] = {0};  // valores possiveis de 0 a 99 pois a base importa aqui

    // Contar ocorrencias
    for (int i = 0; i < tam; i++)
        cont[v[i]]++;

    // Acumulador para posicoes
    for (int i = 1; i < 100; i++)
        cont[i] += cont[i - 1];

    // Construir vetor ordenado
    for (int i = tam - 1; i >= 0; i--) {
        saida[cont[v[i]] - 1] = v[i];
        cont[v[i]]--;
    }

    // Copiar resultado de volta
    for (int i = 0; i < tam; i++)
        v[i] = saida[i];

    free(saida);
}

void radixSortProcess(int *v, int tam) {
    countingSort(v, tam);  
}

void radixSortSequencial(int *v, int tam) {
    radixSortProcess(v, tam);
}

int main() {
    srand(time(NULL));
    int tam = 100000000;

    for (int i = 0; i < 100; i++) {
        printf("\nDESORDENADO #%d\n", i + 1);
        int *v = gerarArray(tam);
        printarArrayLimitado(v, tam);

        double inicio = omp_get_wtime();
        radixSortSequencial(v, tam);
        double fim = omp_get_wtime();

        printf("\nORDENADO #%d\n", i + 1);
        printarArrayLimitado(v, tam);
        printf("Tempo de execucao: %.6f segundos\n", fim - inicio);

        free(v);

    }

    return 0;
}
