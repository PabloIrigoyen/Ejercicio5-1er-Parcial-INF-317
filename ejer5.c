#include "stdio.h"
#include "stdlib.h"
#include "mpi.h"

void mulMatriz(int **matriz, int *vector, int *resultado, int N) {
    for (int i = 0; i < N; i++) {
        resultado[i] = 0;
        for (int j = 0; j < N; j++) {
            resultado[i] += matriz[i][j] * vector[j];
        }
    }
}

int main(int argc, char *argv[]) {
    int rank, size, N;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        printf("Ingrese la dimension de la matriz cuadrada: ");
        scanf("%d", &N);
        int **matriz = (int **)malloc(N * sizeof(int *));
        for (int i = 0; i < N; i++) {
            matriz[i] = (int *)malloc(N * sizeof(int));
        }

        int *vector = (int *)malloc(N * sizeof(int));
        int *resultado = (int *)malloc(N * sizeof(int));

        printf("Ingrese los elementos de la matriz:\n");
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                scanf("%d", &matriz[i][j]);
            }
        }

        printf("Ingrese los elementos del vector:\n");
        for (int i = 0; i < N; i++) {
            scanf("%d", &vector[i]);
        }
        for (int i = 1; i < size; i++) {
            MPI_Send(&N, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            for (int j = 0; j < N; j++) {
                MPI_Send(matriz[j], N, MPI_INT, i, 0, MPI_COMM_WORLD);
            }
            MPI_Send(vector, N, MPI_INT, i, 0, MPI_COMM_WORLD);
        }

        mulMatriz(matriz, vector, resultado, N);
        for (int i = 1; i < size; i++) {
            MPI_Recv(resultado, N, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for (int j = 0; j < N; j++) {
                printf("%d ", resultado[j]);
            }
            printf("\n");
        }

        for (int i = 0; i < N; i++) {
            free(matriz[i]);
        }
        free(matriz);
        free(vector);
        free(resultado);
    } else {
        MPI_Recv(&N, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        int **matriz = (int **)malloc(N * sizeof(int *));
        for (int i = 0; i < N; i++) {
            matriz[i] = (int *)malloc(N * sizeof(int));
            MPI_Recv(matriz[i], N, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        int *vector = (int *)malloc(N * sizeof(int));
        MPI_Recv(vector, N, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        int *resultado = (int *)malloc(N * sizeof(int));
        multiplicar_matriz_vector(matriz, vector, resultado, N);

        MPI_Send(resultado, N, MPI_INT, 0, 0, MPI_COMM_WORLD);
        for (int i = 0; i < N; i++) {
            free(matriz[i]);
        }
        free(matriz);
        free(vector);
        free(resultado);
    }

    MPI_Finalize();
    return 0;
}
