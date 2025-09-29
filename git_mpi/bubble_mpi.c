#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>


void gerarArray(int *v, long long tam) {
    for (long long i = 0; i < tam; i++) {
        v[i] = rand() % 100;
    }
}

void printarArrayExtenso(int *v, long long tam) {
    if (tam <= 100) {
        for (long long i = 0; i < tam; i++) printf("%d ", v[i]);
        printf("\n");
    } else {
        for (long long i = 0; i < 50; i++) printf("%d ", v[i]);
        printf("... ");
        for (long long i = tam - 50; i < tam; i++) printf("%d ", v[i]);
        printf(" (total %lld elementos)\n", tam);
    }
}

void oddEvenSortMPI(int *v, long long tam, int rank, int size) {
    int *counts = malloc(size * sizeof(int));
    int *displs = malloc(size * sizeof(int));
    long long base = tam / size;
    long long resto = tam % size;

    for (int i = 0; i < size; i++) {
        counts[i] = base + (i < resto ? 1 : 0);
        displs[i] = (i == 0) ? 0 : displs[i - 1] + counts[i - 1];
    }

    int local_n = counts[rank];
    int *local_v = malloc(local_n * sizeof(int));

    MPI_Scatterv(v, counts, displs, MPI_INT, local_v, local_n, MPI_INT, 0, MPI_COMM_WORLD);

    for (long long phase = 0; phase < tam; phase++) {
        int start = phase % 2;
        for (int j = start; j < local_n - 1; j += 2) {
            if (local_v[j] > local_v[j + 1]) {
                int temp = local_v[j];
                local_v[j] = local_v[j + 1];
                local_v[j + 1] = temp;
            }
        }

        if (phase % 2 == 0) {
            if (rank % 2 == 0 && rank + 1 < size) {
                MPI_Send(&local_v[local_n - 1], 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
                int recv_val;
                MPI_Recv(&recv_val, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                if (local_v[local_n - 1] > recv_val) {
                    int temp = local_v[local_n - 1];
                    local_v[local_n - 1] = recv_val;
                    recv_val = temp;
                }
                MPI_Send(&recv_val, 1, MPI_INT, rank + 1, 1, MPI_COMM_WORLD);
                MPI_Recv(&local_v[local_n - 1], 1, MPI_INT, rank + 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
            else if (rank % 2 == 1) {
                int recv_val;
                MPI_Recv(&recv_val, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Send(&local_v[0], 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD);
                if (recv_val > local_v[0]) {
                    int temp = recv_val;
                    recv_val = local_v[0];
                    local_v[0] = temp;
                }
                MPI_Recv(&local_v[0], 1, MPI_INT, rank - 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Send(&recv_val, 1, MPI_INT, rank - 1, 1, MPI_COMM_WORLD);
            }
        } else {

            if (rank % 2 == 1 && rank + 1 < size) {
                MPI_Send(&local_v[local_n - 1], 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
                int recv_val;
                MPI_Recv(&recv_val, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                if (local_v[local_n - 1] > recv_val) {
                    int temp = local_v[local_n - 1];
                    local_v[local_n - 1] = recv_val;
                    recv_val = temp;
                }
                MPI_Send(&recv_val, 1, MPI_INT, rank + 1, 1, MPI_COMM_WORLD);
                MPI_Recv(&local_v[local_n - 1], 1, MPI_INT, rank + 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
            else if (rank % 2 == 0 && rank > 0) {
                int recv_val;
                MPI_Recv(&recv_val, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Send(&local_v[0], 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD);
                if (recv_val > local_v[0]) {
                    int temp = recv_val;
                    recv_val = local_v[0];
                    local_v[0] = temp;
                }
                MPI_Recv(&local_v[0], 1, MPI_INT, rank - 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Send(&recv_val, 1, MPI_INT, rank - 1, 1, MPI_COMM_WORLD);
            }
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }

    MPI_Gatherv(local_v, local_n, MPI_INT, v, counts, displs, MPI_INT, 0, MPI_COMM_WORLD);

    free(local_v);
    free(counts);
    free(displs);
}

int main(int argc, char *argv[]) {
    int rank, size;
    long long tam = 1000000; 
    int *v = NULL;
    double inicio, fim, tempo_total = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    for (int i = 0; i < 100; i++) { 
        if (rank == 0) {
            v = malloc(tam * sizeof(int));
            srand(time(NULL) + i);
            gerarArray(v, tam);
            printf("\nIteracao %d - Array desordenado:\n", i + 1);
            printarArrayExtenso(v, tam);
        }

        MPI_Barrier(MPI_COMM_WORLD);
        if (rank == 0) inicio = MPI_Wtime();

        oddEvenSortMPI(v, tam, rank, size);

        if (rank == 0) {
            fim = MPI_Wtime();
            tempo_total += (fim - inicio);
            printf("Array ordenado:\n");
            printarArrayExtenso(v, tam);
            printf("\nTempo da iteracao %d: %.6f segundos\n", i + 1, fim - inicio);
            free(v);
        }

        MPI_Barrier(MPI_COMM_WORLD);
    }

    if (rank == 0) {
        printf("\nTempo medio por iteracao: %.6f segundos\n", tempo_total / 100.0);
    }

    MPI_Finalize();
    return 0;
}
