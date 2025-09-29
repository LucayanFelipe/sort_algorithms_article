#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <mpi.h>

int* gerarArray(int tam) {
    int *v = (int*)malloc(tam * sizeof(int));
    for (int i = 0; i < tam; i++) {
        v[i] = rand() % tam;
    }
    return v;
}

void printarArrayLimitado(int *v, int tam, int limite) {
    for (int i = 0; i < tam && i < limite; i++) {
        printf("%d ", v[i]);
    }
    printf("\n");
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


int main(int argc, char *argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int tam = 100000;  
    int iter = 100;       
    double somaTempos = 0.0;

    for (int it = 0; it < iter; it++) {
        int *v = NULL;

        if (rank == 0) {
            v = gerarArray(tam);
        }

        int tamLocal = tam / size;
        int *local = (int*)malloc(tamLocal * sizeof(int));
        MPI_Scatter(v, tamLocal, MPI_INT, local, tamLocal, MPI_INT, 0, MPI_COMM_WORLD);
        double inicio = MPI_Wtime();

        insertionSort(local, 0, tamLocal - 1);

        int passo = 1;
        int *recebido = NULL;
        int tamRecebido = 0;

        while (passo < size) {
            if (rank % (2 * passo) == 0) {
                if (rank + passo < size) {
                    MPI_Recv(&tamRecebido, 1, MPI_INT, rank + passo, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    recebido = (int*)malloc(tamRecebido * sizeof(int));
                    MPI_Recv(recebido, tamRecebido, MPI_INT, rank + passo, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                    int *novo = mergeArrays(local, tamLocal, recebido, tamRecebido);
                    free(local);
                    free(recebido);
                    local = novo;
                    tamLocal += tamRecebido;
                }
            } else {
                int destino = rank - passo;
                MPI_Send(&tamLocal, 1, MPI_INT, destino, 0, MPI_COMM_WORLD);
                MPI_Send(local, tamLocal, MPI_INT, destino, 0, MPI_COMM_WORLD);
                free(local);
                break; 
            }
            passo *= 2;
        }

        double fim = MPI_Wtime();

        if (rank == 0) {
            double tempo = fim - inicio;
            somaTempos += tempo;
            printf("Iteração %d: Tempo = %.4f s\n", it + 1, tempo);

            printf("Array ordenado (primeiros 20): ");
            printarArrayLimitado(local, tamLocal, 20);

            free(local);
            free(v);
        }
    }

    if (rank == 0) {
        printf("Tempo médio após %d iterações = %.4f s\n", iter, somaTempos / iter);
    }

    MPI_Finalize();
    return 0;
}
