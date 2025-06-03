// cannon_multiply_parallel.c
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define MATRIX_SIZE 1024
#define MAX_VAL 10
#define MIN_VAL 1

void fill_random(int *mat, int elements) {
    for (int i = 0; i < elements; ++i)
        mat[i] = rand() % (MAX_VAL - MIN_VAL + 1) + MIN_VAL;
}

void local_multiply(int *A, int *B, int *C, int block) {
    for (int i = 0; i < block; ++i)
        for (int k = 0; k < block; ++k)
            for (int j = 0; j < block; ++j)
                C[i * block + j] += A[i * block + k] * B[k * block + j];
}

void shift_matrix(int *mat, int block, int count, int direction, MPI_Comm comm2d) {
    int src, dst;
    MPI_Cart_shift(comm2d, direction, 1, &src, &dst);
    MPI_Sendrecv_replace(mat, block * block, MPI_INT, dst, 0, src, 0, comm2d, MPI_STATUS_IGNORE);
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int q = (int)sqrt(size);
    if (q * q != size) {
        if (rank == 0) fprintf(stderr, "El número de procesos debe ser un cuadrado perfecto.\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    int dims[2] = {q, q}, periods[2] = {1, 1}, coords[2];
    MPI_Comm comm2d;
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 1, &comm2d);
    MPI_Cart_coords(comm2d, rank, 2, coords);

    int n = MATRIX_SIZE;
    int block = n / q;

    int *Ablock = malloc(block * block * sizeof(int));
    int *Bblock = malloc(block * block * sizeof(int));
    int *Cblock = calloc(block * block, sizeof(int));

    int *A = NULL, *B = NULL;
    if (rank == 0) {
        A = malloc(n * n * sizeof(int));
        B = malloc(n * n * sizeof(int));
        fill_random(A, n * n);
        fill_random(B, n * n);
    }

    int *Ascat = NULL, *Bscat = NULL;
    if (rank == 0) {
        Ascat = malloc(size * block * block * sizeof(int));
        Bscat = malloc(size * block * block * sizeof(int));
        for (int proc = 0; proc < size; ++proc) {
            int i = proc / q, j = proc % q;
            for (int bi = 0; bi < block; ++bi)
                memcpy(&Ascat[proc * block * block + bi * block],
                       &A[(i * block + bi) * n + j * block],
                       block * sizeof(int));
            for (int bi = 0; bi < block; ++bi)
                memcpy(&Bscat[proc * block * block + bi * block],
                       &B[(i * block + bi) * n + j * block],
                       block * sizeof(int));
        }
    }

    MPI_Scatter(Ascat, block * block, MPI_INT, Ablock, block * block, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(Bscat, block * block, MPI_INT, Bblock, block * block, MPI_INT, 0, MPI_COMM_WORLD);
    if (rank == 0) { free(Ascat); free(Bscat); }

    for (int i = 0; i < coords[0]; ++i) shift_matrix(Ablock, block, q, 1, comm2d);
    for (int i = 0; i < coords[1]; ++i) shift_matrix(Bblock, block, q, 0, comm2d);

    MPI_Barrier(MPI_COMM_WORLD);
    double t0 = MPI_Wtime();

    for (int step = 0; step < q; ++step) {
        local_multiply(Ablock, Bblock, Cblock, block);
        shift_matrix(Ablock, block, q, 1, comm2d);
        shift_matrix(Bblock, block, q, 0, comm2d);
    }

    double elapsed = MPI_Wtime() - t0;
    if (rank == 0) printf("Cannon completado en %.6f segundos\n", elapsed);

    free(Ablock); free(Bblock); free(Cblock);
    if (rank == 0) { free(A); free(B); }
    MPI_Comm_free(&comm2d);
    MPI_Finalize();
    return 0;
}