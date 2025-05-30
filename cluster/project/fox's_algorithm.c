// fox_multiply.c
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define N 1024
#define MAX_VAL 10
#define MIN_VAL 1

void fill_random(int *mat, int elements) {
    for (int i = 0; i < elements; ++i)
        mat[i] = rand() % (MAX_VAL - MIN_VAL + 1) + MIN_VAL;
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int q = (int)sqrt(size);
    if (q * q != size) {
        if (rank == 0) fprintf(stderr, "El número de procesos debe ser cuadrado perfecto.\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    int n = N;
    int block = n / q;

    int *Ablock = malloc(block * block * sizeof(int));
    int *Bblock = malloc(block * block * sizeof(int));
    int *Cblock = calloc(block * block, sizeof(int));
    int *tempA = malloc(block * block * sizeof(int));

    // Inicialización omitida, como en Cannon

    // Crear comunicadores fila y columna (ver documentación MPI_Cart_create)
    // Aquí solo representamos la idea, la implementación real requiere más código

    MPI_Barrier(MPI_COMM_WORLD);
    double t0 = MPI_Wtime();

    for (int step = 0; step < q; ++step) {
        // "Broadcast" del bloque de A apropiado dentro de la fila
        // MPI_Bcast(tempA, block*block, MPI_INT, root_fila, comm_fila);
        // Multiplicación local
        for (int i = 0; i < block; ++i)
            for (int k = 0; k < block; ++k)
                for (int j = 0; j < block; ++j)
                    Cblock[i * block + j] +=
                        tempA[i * block + k] * Bblock[k * block + j];

        // Rotar Bblocks arriba en columna (Sendrecv)
    }

    double elapsed = MPI_Wtime() - t0;

    if (rank == 0) printf("Fox completado en %.6f segundos\n", elapsed);

    free(Ablock); free(Bblock); free(Cblock); free(tempA);
    MPI_Finalize();
    return 0;
}