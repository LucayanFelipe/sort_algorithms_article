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

void heapar(int *v, int tam, int i) {
    while (1) {
        int maior = i;
        int l = 2 * i + 1;
        int r = 2 * i + 2;

        if (l < tam && v[l] > v[maior]) maior = l;
        if (r < tam && v[r] > v[maior]) maior = r;

        if (maior == i) break;

        int troca = v[i];
        v[i] = v[maior];
        v[maior] = troca;
        i = maior;
    }
}



void heapSortProcess(int *v, int tam) {
    for(int i=tam/2-1;i>=0;i--) {
        heapar(v,tam,i);
    }
    for(int i= tam-1;i>0;i--) {
        int troca = v[0]; //qnd chamar o heapar dnv a primeira pos sempre vai ter o maior elemento e joga pro final
        v[0] = v[i];
        v[i] = troca;
        heapar(v,i,0);
    }

}


void heapSortSequencial (int *v, int tam) {
    heapSortProcess(v,tam);
}



int main() {
    srand(time(NULL));
    int tam = 100000000;

    for (int i = 0; i < 100; i++) {
        printf("\nDESORDENADO #%d\n  tamanho (%d)\n", i + 1, tam);
        int *v = gerarArray(tam);
        printarArrayLimitado(v, tam);

        double inicio = omp_get_wtime();
        heapSortSequencial(v, tam);
        double fim = omp_get_wtime();

        printf("\nORDENADO #%d\n", i + 1);
        printarArrayLimitado(v, tam);
        printf("Tempo de execucao: %.6f segundos\n", fim - inicio);

        free(v);
    }

    return 0;
}


/*  ESTRUTURA PARA OS ALGORITMOS OPENMP
    srand(time(NULL));
    int tam=10;
    //for(int i=0;i<20;i++) 
    //{
    //printf("\nDESORDENADO #%d",i);
    printf("\nDESORDENADO");
    int *v = gerarArray(tam);
    printarArray(v,tam);
    bubbleSortMPI(v,tam);
    printf("\nTIPO Sort (PARALELO)");
    printarArray(v,tam);
    //sleep(2);
    //}
    return 0;
*/


