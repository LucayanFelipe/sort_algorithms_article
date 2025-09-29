#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

int* gerarArray(int tam) {
    int *v;
    v = (int*)malloc(tam*sizeof(int));
    for(int i=0;i<tam;i++) {
       v[i] = rand()%100;
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



int getMaxElementArrayParallel(int *v, int tam) {
    int max = v[0];

    #pragma omp parallel for reduction(max:max)
    for (int i = 1; i < tam; i++) {
        if (v[i] > max) {
            max = v[i];
        }
    }
    
    return max;
}

void countingSortParallel(int *v, int tam, int dec, int range) {
    int *saida = (int*) calloc(tam, sizeof(int));
    int *cont = (int*) calloc(range, sizeof(int)); 
    
    #pragma omp parallel for reduction(+:cont[:range])
    for (int i = 0; i < tam; i++) {
        cont[(v[i] / dec) % range]++;
    }
    
    for (int i = 1; i < range; i++) {
        cont[i] += cont[i - 1];
    }

    for (int i = tam - 1; i >= 0; i--) {
        int index = --cont[(v[i] / dec) % range];
        saida[index] = v[i];
    }

    #pragma omp parallel for
    for (int i = 0; i < tam; i++) {
        v[i] = saida[i];
    }

    free(saida);
    free(cont);
}

void radixSortProcessParallel(int *v, int tam) {
    int max = getMaxElementArrayParallel(v, tam);
    int range = max + 1; 
    for (int i = 1; max / i > 0; i *= range) {
        countingSortParallel(v, tam, i, range);
    }
}

void radixSortParalelo(int *v, int tam) {
    radixSortProcessParallel(v, tam);
}


int main() {
    srand(time(NULL));
    int tam = 100000000;

    for (int i = 0; i < 100; i++) {
        printf("\nDESORDENADO #%d\n", i + 1);
        int *v = gerarArray(tam);
        printarArrayLimitado(v, tam);

        double inicio = omp_get_wtime();
        radixSortParalelo(v, tam);
        double fim = omp_get_wtime();

        printf("\nORDENADO #%d\n", i + 1);
        printarArrayLimitado(v, tam);
        printf("Tempo de execucao: %.6f segundos\n", fim - inicio);

        free(v);

    }

    return 0;
}




