#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

// Gera um array aleatario de tamanho tam
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

// Imprime ate 100 elementos do array (para grandes vetores, mostra inicio e fim)
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
    int *pilha = (int *)malloc((right - left + 1) * sizeof(int) * 2);
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



void quickSortSequencial(int *v, int tam) {
    quickSortProcess(v, 0, tam - 1);
}

int main() {
    srand(time(NULL));
    double tempo_total=0;
    int tam = 100000000;

    for (int i = 0; i < 100; i++) {
        printf("\nDESORDENADO #%d\n  tamanho (%d)\n", i + 1, tam);
        int *v = gerarArray(tam);
        printarArrayLimitado(v, tam);

        double inicio = omp_get_wtime();
        quickSortSequencial(v, tam);
        double fim = omp_get_wtime();

        printf("\nORDENADO #%d\n", i + 1);
        printarArrayLimitado(v, tam);
        printf("Tempo de execucao: %.6f segundos\n", fim - inicio);
        tempo_total += (fim - inicio);
        free(v);
    }
              printf("\nTempo medio por iteracao: %.6f segundos\n", tempo_total / 100.0);

    return 0;
}
