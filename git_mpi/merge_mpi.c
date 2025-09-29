#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

void gerarArray(int *v, int tam) {
    for (int i = 0; i < tam; i++) {
        v[i] = rand() % 100;  // números aleatórios entre 0 e 99
    }
}

void printarArray(int *v, int tam) {
    for (int i = 0; i < tam; i++) {
        printf("%d ", v[i]);
    }
    printf("\n");
}

void merge(int *v, int *temp, int esquerda, int meio, int direita) {
    int i = esquerda;
    int j = meio + 1;
    int k = esquerda;
    
    while (i <= meio && j <= direita) {
        if (v[i] <= v[j]) {
            temp[k++] = v[i++];
        } else {
            temp[k++] = v[j++];
        }
    }

    while (i <= meio) {
        temp[k++] = v[i++];
    }

    while (j <= direita) {
        temp[k++] = v[j++];
    }

    for (i = esquerda; i <= direita; i++) {
        v[i] = temp[i];
    }
}

void mergeSort(int *v, int *temp, int esquerda, int direita) {
    if (esquerda < direita) {
        int meio = (esquerda + direita) / 2;
        
        mergeSort(v, temp, esquerda, meio);
        mergeSort(v, temp, meio + 1, direita);
        
        merge(v, temp, esquerda, meio, direita);
    }
}

void mergeSortMPI(int *v, int tam, int rank, int size) {
    int *temp = (int*) malloc(tam * sizeof(int));

    int chunk_size = tam / size;
    int *sub_array = (int*) malloc(chunk_size * sizeof(int));

    MPI_Scatter(v, chunk_size, MPI_INT, sub_array, chunk_size, MPI_INT, 0, MPI_COMM_WORLD);

    mergeSort(sub_array, temp, 0, chunk_size - 1);

    MPI_Gather(sub_array, chunk_size, MPI_INT, v, chunk_size, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        mergeSort(v, temp, 0, tam - 1);
    }

    free(sub_array);
    free(temp);
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

    mergeSortMPI(v, tam, rank, size);

    if (rank == 0) {
        printf("Array ordenado: \n");
        printarArray(v, tam);
        free(v);
    }
    MPI_Finalize();
    return 0;
}

