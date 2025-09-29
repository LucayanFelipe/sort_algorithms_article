#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

void gerarArray(int *v, int tam) {
    for (int i = 0; i < tam; i++) {
        v[i] = rand() % 100; // números aleatórios de 0 a 99
    }
}

void printarArray(int *v, int tam) {
    for (int i = 0; i < tam; i++) {
        printf("%d ", v[i]);
    }
    printf("\n");
}

int partition(int *v, int low, int high) {
    int pivot = v[high];
    int i = (low - 1); 

    for (int j = low; j < high; j++) {
        if (v[j] <= pivot) {
            i++;
            int temp = v[i];
            v[i] = v[j];
            v[j] = temp;
        }
    }
    int temp = v[i + 1];
    v[i + 1] = v[high];
    v[high] = temp;
    return i + 1;
}

void quickSort(int *v, int low, int high) {
    if (low < high) {
        int pi = partition(v, low, high);
        quickSort(v, low, pi - 1);
        quickSort(v, pi + 1, high);
    }
}


void quickSortMPI(int *v, int tam, int rank, int size) {
    int local_n = tam / size;
    int *local_v = (int*) malloc(local_n * sizeof(int));
    MPI_Scatter(v, local_n, MPI_INT, local_v, local_n, MPI_INT, 0, MPI_COMM_WORLD);

    printf("Rank %d processando os elementos de %d a %d\n", rank, rank * local_n, (rank + 1) * local_n - 1);
    printf("Rank %d, parte do vetor: ", rank);
    printarArray(local_v, local_n);

    printf("\n");
    quickSort(local_v, 0, local_n - 1);

    MPI_Gather(local_v, local_n, MPI_INT, v, local_n, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        quickSort(v, 0, tam - 1);
    }

    free(local_v);
}

int main(int argc, char** argv) {
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
        printf("\n");
    }

    quickSortMPI(v, tam, rank, size);

    if (rank == 0) {
        printf("Array ordenado: \n");
        printarArray(v, tam);
        free(v);
    }

    MPI_Finalize();
    return 0;
}

