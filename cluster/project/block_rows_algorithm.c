/*
 * Distributed square matrix multiplication using MPI (MPICH)
 * -----------------------------------------------------------
 *  - Author: Angel Navajas, Daniel Sanchez
 *  - Purpose: Practice C and distributed programming with MPICH in a matrix multiplication exercise .
 *  - Features:
 *      • Random initialization of A and B with integers in [1,10]
 *      • Block‑row distribution of A, full broadcast of B
 *      • Pure computation timed with MPI_Wtime (excludes I/O & init)
 *      • Matrix order N must be a multiple of 8; default 1024 (can be
 *        overridden at compile‑time with -DN=<size> or at runtime by
 *        providing <size> as argv[1])
 *
 *  Build & run examples
 *  --------------------
 *      mpicc -O3 -DN=1024 -o matmul mpi_matrix_multiply.c
 *      mpirun -np 8 ./matmul            # uses N from -D or default
 *      mpirun -np 4 ./matmul 2048       # overrides to 2048 at runtime
 *
 *  Notes
 *  -----
 *      • MATRIX_SIZE must be divisible by the number of processes; otherwise the
 *        program aborts.
 *      • The reported time is the maximum across all ranks so it reflects
 *        total wall‑clock runtime.
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_VAL 10
#define MIN_VAL 1
#ifndef MATRIX_SIZE
#define MATRIX_SIZE 1024
#endif

/*------------------------------------------------------------*/
static void fill_random(int *mat, int elements) {
    for (int i = 0; i < elements; ++i) {
        mat[i] = rand() % (MAX_VAL - MIN_VAL + 1) + MIN_VAL;
    }
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    /* Determine matrix order */
    if (argc > 1) {
        MATRIX_SIZE = atoi(argv[1]);
    }

    if (MATRIX_SIZE % 8 != 0) {
        if (rank == 0) fprintf(stderr, "Error: N (%d) must be a multiple of 8.\n", n);
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }
    if (MATRIX_SIZE % size != 0) {
        if (rank == 0) fprintf(stderr, "Error: N (%d) must be divisible by number of processes (%d).\n", n, size);
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    int rows_per_proc = MATRIX_SIZE / size;
    size_t block_elems = (size_t)rows_per_proc * n;

    /* Root allocates full matrices; others just what they need */
    int *A = NULL, *B = NULL, *C = NULL;
    int *local_A = (int *)malloc(block_elems * sizeof(int));
    int *local_C = (int *)calloc(block_elems, sizeof(int));
    if (!local_A || !local_C) {
        fprintf(stderr, "Rank %d: Memory allocation failure.\n", rank);
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    if (rank == 0) {
        A = (int *)malloc((size_t)MATRIX_SIZE * MATRIX_SIZE * sizeof(int));
        B = (int *)malloc((size_t)MATRIX_SIZE * MATRIX_SIZE * sizeof(int));
        C = (int *)malloc((size_t)MATRIX_SIZE * MATRIX_SIZE * sizeof(int));
        if (!A || !B || !C) {
            fprintf(stderr, "Root: Memory allocation failure.\n");
            MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
        }
        srand((unsigned)time(NULL));
        fill_random(A, MATRIX_SIZE * n);
        fill_random(B, MATRIX_SIZE * n);
    }

    /* Broadcast B to everyone */
    if (rank != 0) {
        B = (int *)malloc((size_t)MATRIX_SIZE * MATRIX_SIZE * sizeof(int));
    }
    MPI_Bcast(B, MATRIX_SIZE * n, MPI_INT, 0, MPI_COMM_WORLD);

    /* Scatter rows of A */
    MPI_Scatter(A, (int)block_elems, MPI_INT, local_A, (int)block_elems, MPI_INT, 0, MPI_COMM_WORLD);

    /* -------------------------------------------------------- */
    /*        Start timing JUST the multiplication phase        */
    /* -------------------------------------------------------- */
    MPI_Barrier(MPI_COMM_WORLD);
    double t0 = MPI_Wtime();

    for (int i = 0; i < rows_per_proc; ++i) {
        for (int k = 0; k < MATRIX_SIZE; ++k) {
            int a_ik = local_A[i * MATRIX_SIZE + k];
            for (int j = 0; j < MATRIX_SIZE; ++j) {
                local_C[i * MATRIX_SIZE + j] += a_ik * B[k * MATRIX_SIZE + j];
            }
        }
    }

    double local_elapsed = MPI_Wtime() - t0;

    /* -------------------------------------------------------- */
    /*        End of multiplication phase                       */
    /* -------------------------------------------------------- */

    /* Gather times and results */
    double elapsed;
    MPI_Reduce(&local_elapsed, &elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    MPI_Gather(local_C, (int)block_elems, MPI_INT, C, (int)block_elems, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Matrix multiplication %dx%d completed in %.6f seconds across %d process(es).\n", n, n, elapsed, size);
        /* Optional: verify correctness or write C to disk */
        free(A);
        free(B);
        free(C);
    } else {
        free(B);
    }

    free(local_A);
    free(local_C);
    MPI_Finalize();
    return 0;
}