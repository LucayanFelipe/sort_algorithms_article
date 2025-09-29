#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>


void gerarArray(int *v, int tam) {
    for (int i = 0; i < tam; i++) {
        v[i] = rand() % 100; 
    }
}

void printarArray(int *v, int tam) {
    for (int i = 0; i < tam; i++) {
        printf("%d ", v[i]);
    }
    printf("\n");
}

int getMaxElementArray(int *v, int tam) {
    int max = v[0];
    for (int i = 1; i < tam; i++) {
        if (v[i] > max) {
            max = v[i];
        }
    }
    return max;
}

void countingSort(int *v, int tam, int exp) {
    int saida[tam];
    int cont[10] = {0};
    for (int i = 0; i < tam; i++)
        cont[(v[i] / exp) % 10]++;
    for (int i = 1; i < 10; i++)
        cont[i] += cont[i - 1];
    for (int i = tam - 1; i >= 0; i--) {
        saida[cont[(v[i] / exp) % 10] - 1] = v[i];
        cont[(v[i] / exp) % 10]--;
    }
    for (int i = 0; i < tam; i++)
        v[i] = saida[i];
}

void radixSort(int *v, int tam) {
    int max = getMaxElementArray(v, tam);

    for (int exp = 1; max / exp > 0; exp *= 10)
        countingSort(v, tam, exp);
}

void radixSortMPI(int *v, int tam, int rank, int size) {
    int chunk_size = tam / size;
    int *sub_v = (int*) malloc(chunk_size * sizeof(int));

    MPI_Scatter(v, chunk_size, MPI_INT, sub_v, chunk_size, MPI_INT, 0, MPI_COMM_WORLD);
    radixSort(sub_v, chunk_size);
    MPI_Gather(sub_v, chunk_size, MPI_INT, v, chunk_size, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        radixSort(v, tam);
    }

    free(sub_v);
}

int main(int argc, char *argv[]) {
    int rank, size;
    int tam = 100000000; 
    int *v = NULL;


    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        v = (int*) malloc(tam * sizeof(int));
        gerarArray(v, tam);
        printf("Array desordenado: \n");
        printarArray(v, tam);
    }

    radixSortMPI(v, tam, rank, size);

    if (rank == 0) {
        printf("Array ordenado: \n");
        printarArray(v, tam);
        free(v);
    }

    MPI_Finalize();
    return 0;
}

