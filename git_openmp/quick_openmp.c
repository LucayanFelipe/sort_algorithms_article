#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>


int* gerarArray(int tam) {
    int *v = (int*)malloc(tam * sizeof(int));
    if (v == NULL) {
        fprintf(stderr, "Erro ao alocar memoria para vetor de tamanho %d.\n", tam);
        exit(EXIT_FAILURE);
    }
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

int particiona(int *v, int low, int high) {
    int mid = (low + high) / 2;
    
    if (v[low] > v[mid]) { int tmp = v[low]; v[low] = v[mid]; v[mid] = tmp; }
    if (v[mid] > v[high]) { int tmp = v[mid]; v[mid] = v[high]; v[high] = tmp; }
    if (v[low] > v[mid]) { int tmp = v[low]; v[low] = v[mid]; v[mid] = tmp; }
    
    int tmp = v[mid];
    v[mid] = v[high-1];
    v[high-1] = tmp;
    
    int pivo = v[high-1];
    int i = low;
    int j = high-1;
    
    while (1) {
        while (v[++i] < pivo);
        while (v[--j] > pivo);
        if (i >= j) break;
        tmp = v[i]; v[i] = v[j]; v[j] = tmp;
    }
    
    tmp = v[i]; v[i] = v[high-1]; v[high-1] = tmp;
    return i;
}

void quickSortProcess(int *v, int left, int right) {
    int tam = right - left + 1;
    int *pilha = (int *)malloc(tam * sizeof(int) * 2);
    if (pilha == NULL) {
        fprintf(stderr, "Erro ao alocar memoria para pilha.\n");
        exit(EXIT_FAILURE);
    }

    int topo = -1;
    pilha[++topo] = left;
    pilha[++topo] = right;

    while (topo >= 0) {
        right = pilha[topo--];
        left = pilha[topo--];

        int pos = particiona(v, left, right);

        if (pos - 1 > left) {
            pilha[++topo] = left;
            pilha[++topo] = pos - 1;
        }

        if (pos + 1 < right) {
            pilha[++topo] = pos + 1;
            pilha[++topo] = right;
        }
    }

    free(pilha);
}

void quickSortParallel(int *v, int left, int right, int limite) {
    if (right - left < limite) {
        quickSortProcess(v, left, right);
        return;
    }

    int pos = particiona(v, left, right);

    #pragma omp task shared(v)
    quickSortParallel(v, left, pos - 1, limite);

    #pragma omp task shared(v)
    quickSortParallel(v, pos + 1, right, limite);
}


void quickSortParalelo(int *v, int tam) {
    int limite = 10000; 
    #pragma omp parallel
    {
        #pragma omp single nowait
        quickSortParallel(v, 0, tam - 1, limite);
    }
}

int main() {
    srand(time(NULL));
    int tam = 100000000; 

    for (int i = 0; i < 100; i++) {
        printf("\nDESORDENADO #%d\n  tamanho (%d)\n", i + 1, tam);
        int *v = gerarArray(tam);
        printarArrayLimitado(v, tam);

        double inicio = omp_get_wtime();
        quickSortParalelo(v, tam);
        double fim = omp_get_wtime();

        printf("\nORDENADO #%d\n", i + 1);
        printarArrayLimitado(v, tam);
        printf("Tempo de execucao: %.6f segundos\n", fim - inicio);

        free(v);
    }

    return 0;
}
