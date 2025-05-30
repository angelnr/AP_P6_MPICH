// cannon_multiply.c
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MATRIX_SIZE 1024  // tamaño de matriz, cuadrado
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

    // Suponemos size = q*q (ej. 4, 9, 16...) para Cannon
    int q = (int)sqrt(size);
    if (q * q != size) {
        if (rank == 0) fprintf(stderr, "El número de procesos debe ser cuadrado perfecto.\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    int n = MATRIX_SIZE;
    int block = n / q;  // tamaño de bloque local

    int *Ablock = malloc(block * block * sizeof(int));
    int *Bblock = malloc(block * block * sizeof(int));
    int *Cblock = calloc(block * block, sizeof(int));

    // Solo root inicializa y reparte
    int *A = NULL, *B = NULL;
    if (rank == 0) {
        A = malloc(n * n * sizeof(int));
        B = malloc(n * n * sizeof(int));
        fill_random(A, n * n);
        fill_random(B, n * n);
    }

    // Distribute blocks of A and B to all processes (no hay Scatterv, pseudo código)
    // Supón que se reparte el bloque correspondiente a cada rank
    // (para pruebas: pon todas las Ablock/Bblock en 1)

    // Inicialización "skew": desplazar bloques de A y B
    // (debería hacerse con Sendrecv en mallas, aquí se omite por claridad)

    MPI_Barrier(MPI_COMM_WORLD);
    double t0 = MPI_Wtime();

    // q pasos: en cada paso, multiplica local y gira bloques
    for (int step = 0; step < q; ++step) {
        // Multiplicación local: Cblock += Ablock * Bblock
        for (int i = 0; i < block; ++i)
            for (int k = 0; k < block; ++k)
                for (int j = 0; j < block; ++j)
                    Cblock[i * block + j] +=
                        Ablock[i * block + k] * Bblock[k * block + j];

        // Rotar bloques A a la izq, B hacia arriba (Sendrecv con malla 2D)
        // ... aquí se omite, solo indicarías la idea:
        // MPI_Sendrecv_replace(Ablock, ..., left, ..., right, ...);
        // MPI_Sendrecv_replace(Bblock, ..., up, ..., down, ...);
    }

    double elapsed = MPI_Wtime() - t0;

    // Podrías recolectar resultados, etc.
    if (rank == 0) printf("Cannon completado en %.6f segundos\n", elapsed);

    free(Ablock); free(Bblock); free(Cblock);
    if (rank == 0) { free(A); free(B); }
    MPI_Finalize();
    return 0;
}