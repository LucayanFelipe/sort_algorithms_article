#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

int* gerarArray(int tam) {
    int *v;
    v = (int*)malloc(tam * sizeof(int));
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
    int left_length = m - l + 1;
    int right_length = r - m;

    int *temp_left = malloc(left_length * sizeof(int));
    int *temp_right = malloc(right_length * sizeof(int));

    for (int i = 0; i < left_length; i++)
        temp_left[i] = v[l + i];
    for (int j = 0; j < right_length; j++)
        temp_right[j] = v[m + 1 + j];

    int i = 0, j = 0, k = l;
    while (i < left_length && j < right_length) {
        if (temp_left[i] <= temp_right[j]) {
            v[k++] = temp_left[i++];
        } else {
            v[k++] = temp_right[j++];
        }
    }

    while (i < left_length) v[k++] = temp_left[i++];
    while (j < right_length) v[k++] = temp_right[j++];

    free(temp_left);
    free(temp_right);
}

void merge_recursion(int *v, int l, int r) {
    if (l < r) {
        int m = l + (r - l) / 2;
        merge_recursion(v, l, m);
        merge_recursion(v, m + 1, r);
        merge(v, l, m, r);
    }
}

void mergeSortSequencial(int *v, int tam) {
    merge_recursion(v, 0, tam - 1);
}

int main() {
    srand(time(NULL));
    int tam = 100000000;

    for (int i = 0; i < 100; i++) {
        printf("\nDESORDENADO #%d  tam(%d)", i + 1,tam);

        int *v = gerarArray(tam);
        printarArrayLimitado(v, tam);

        double inicio = omp_get_wtime();
        mergeSortSequencial(v, tam);
        double fim = omp_get_wtime();

        printf("\nORDENADO #%d", i + 1);
        printarArrayLimitado(v, tam);
        printf("Tempo de execucao: %.3f segundos\n", fim - inicio);

        free(v);
        sleep(2);
    }

    return 0;
}
