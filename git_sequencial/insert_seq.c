#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
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

void insertionSort(int *v, int inicio, int fim) {
    for (int i = inicio + 1; i <= fim; i++) {
        int chave = v[i];
        int j = i - 1;
        while (j >= inicio && v[j] > chave) {
            v[j + 1] = v[j];
            j--;
        }
        v[j + 1] = chave;
    }
}

int* mergeArrays(int *a, int na, int *b, int nb) {
    int *res = (int*)malloc((na + nb) * sizeof(int));
    int i = 0, j = 0, k = 0;
    while (i < na && j < nb) {
        if (a[i] <= b[j]) res[k++] = a[i++];
        else res[k++] = b[j++];
    }
    while (i < na) res[k++] = a[i++];
    while (j < nb) res[k++] = b[j++];
    return res;
}

int main() {
    int tam = 100;  
    int iter = 3;      
    int num_blocos = 4; 
    double somaTempos = 0.0;

    srand(time(NULL));

    for (int it = 0; it < iter; it++) {
        int *v = gerarArray(tam);

        printf("Array desordenado: ");
        printarArrayLimitado(v, tam);

        clock_t inicio = clock();

        int bloco = tam / num_blocos;
        int *blocos[num_blocos];
        int tam_blocos[num_blocos];

        for (int i = 0; i < num_blocos; i++) {
            int inicio_bloco = i * bloco;
            int fim_bloco = (i == num_blocos - 1) ? (tam - 1) : (inicio_bloco + bloco - 1);
            tam_blocos[i] = fim_bloco - inicio_bloco + 1;
            blocos[i] = (int*)malloc(tam_blocos[i] * sizeof(int));
            for (int j = 0; j < tam_blocos[i]; j++) {
                blocos[i][j] = v[inicio_bloco + j];
            }
            insertionSort(blocos[i], 0, tam_blocos[i] - 1);
        }

        int passo = 1;
        while (passo < num_blocos) {
            for (int i = 0; i + passo < num_blocos; i += 2 * passo) {
                int *novo = mergeArrays(blocos[i], tam_blocos[i], blocos[i + passo], tam_blocos[i + passo]);
                tam_blocos[i] += tam_blocos[i + passo];
                free(blocos[i]);
                free(blocos[i + passo]);
                blocos[i] = novo;
            }
            passo *= 2;
        }

        clock_t fim = clock();
        double tempo = (double)(fim - inicio) / CLOCKS_PER_SEC;
        somaTempos += tempo;

        printf("Array ordenado: ");
        printarArrayLimitado(blocos[0], tam);

        printf("Iteração %d: Tempo = %.4f s\n\n", it + 1, tempo);

        free(blocos[0]);
        free(v);
    }
    printf("Tempo médio após %d iterações = %.4f s\n", iter, somaTempos / iter);
    return 0;
}
