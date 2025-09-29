#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

// Função para gerar um array de inteiros aleatórios
void gerarArray(int *v, int tam) {
    for (int i = 0; i < tam; i++) {
        v[i] = rand() % 10;  // números aleatórios entre 0 e 9
    }
}

// Função para imprimir um array
void printarArray(int *v, int tam) {
    for (int i = 0; i < tam; i++) {
        printf("%d ", v[i]);
    }
    printf("\n");
}

// Função para reorganizar o heap
void heapify(int *v, int tam, int i) {
    int maior = i; // Inicializa o maior como raiz
    int esq = 2 * i + 1;
    int dir = 2 * i + 2;

    if (esq < tam && v[esq] > v[maior])
        maior = esq;

    if (dir < tam && v[dir] > v[maior])
        maior = dir;

    if (maior != i) {
        int temp = v[i];
        v[i] = v[maior];
        v[maior] = temp;

        heapify(v, tam, maior);
    }
}

// Função de Heap Sort sequencial
void heapSort(int *v, int tam) {
    // Construção do heap
    for (int i = tam / 2 - 1; i >= 0; i--)
        heapify(v, tam, i);

    // Extração de elementos do heap
    for (int i = tam - 1; i > 0; i--) {
        int temp = v[0];
        v[0] = v[i];
        v[i] = temp;

        heapify(v, i, 0);
    }
}

// Função de Heap Sort paralelizada com MPI
void heapSortMPI(int *v, int tam, int rank, int size) {
    int chunk_size = tam / size;
    int *sub_v = (int*) malloc(chunk_size * sizeof(int));

    // Distribuir partes do vetor para cada processo
    MPI_Scatter(v, chunk_size, MPI_INT, sub_v, chunk_size, MPI_INT, 0, MPI_COMM_WORLD);


    // Reunir os subvetores ordenados no processo 0
    MPI_Gather(sub_v, chunk_size, MPI_INT, v, chunk_size, MPI_INT, 0, MPI_COMM_WORLD);

    // O processo 0 faz uma ordenação final
    if (rank == 0) {
        heapSort(v, tam);
    }

    free(sub_v);
}

int main(int argc, char *argv[]) {
    int rank, size;
    int tam = 100000000; // Tamanho do array
    int *v = NULL;

    // Inicializar o MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // O processo 0 cria o array e distribui
    if (rank == 0) {
        v = (int*) malloc(tam * sizeof(int));
        gerarArray(v, tam);
        printf("Array desordenado: \n");
        printarArray(v, tam);
    }

    // Executar o heapSort com MPI
    heapSortMPI(v, tam, rank, size);

    // O processo 0 imprime o array ordenado
    if (rank == 0) {
        printf("Array ordenado: \n");
        printarArray(v, tam);
        free(v);
    }

    // Finalizar o MPI
    MPI_Finalize();
    return 0;
}

