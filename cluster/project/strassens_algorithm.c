// Corrección de strassen_mpi_parallel.c para permitir menos de 8 procesos y evitar aliasing en MPI_Reduce

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define MATRIX_SIZE 1024
#define MAX_VAL 10
#define MIN_VAL 1
#define THRESHOLD 256

void fill_random(int *m, int n) {
    for (int i = 0; i < n; ++i)
        m[i] = rand() % (MAX_VAL - MIN_VAL + 1) + MIN_VAL;
}

void classic_multiply(int *A, int *B, int *C, int n) {
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            for (int k = 0; k < n; ++k)
                C[i * n + j] += A[i * n + k] * B[k * n + j];
}

void add_mat(int *A, int *B, int *C, int n) {
    for (int i = 0; i < n * n; ++i) C[i] = A[i] + B[i];
}

void sub_mat(int *A, int *B, int *C, int n) {
    for (int i = 0; i < n * n; ++i) C[i] = A[i] - B[i];
}

void split(int *M, int *S, int n, int r, int c) {
    int half = n / 2;
    for (int i = 0; i < half; ++i)
        for (int j = 0; j < half; ++j)
            S[i * half + j] = M[(i + r) * n + (j + c)];
}

void strassen(int *A, int *B, int *C, int n) {
    if (n <= THRESHOLD) {
        classic_multiply(A, B, C, n);
        return;
    }

    int half = n / 2;
    size_t sz = half * half * sizeof(int);

    int *A11 = malloc(sz), *A12 = malloc(sz), *A21 = malloc(sz), *A22 = malloc(sz);
    int *B11 = malloc(sz), *B12 = malloc(sz), *B21 = malloc(sz), *B22 = malloc(sz);

    split(A, A11, n, 0, 0);
    split(A, A12, n, 0, half);
    split(A, A21, n, half, 0);
    split(A, A22, n, half, half);
    split(B, B11, n, 0, 0);
    split(B, B12, n, 0, half);
    split(B, B21, n, half, 0);
    split(B, B22, n, half, half);

    int *local_M[7];
    for (int i = 0; i < 7; ++i)
        local_M[i] = calloc(half * half, sizeof(int));

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int *T1 = malloc(sz), *T2 = malloc(sz);

    for (int i = rank; i < 7; i += size) {
        switch (i) {
            case 0:
                add_mat(A11, A22, T1, half);
                add_mat(B11, B22, T2, half);
                strassen(T1, T2, local_M[0], half);
                break;
            case 1:
                add_mat(A21, A22, T1, half);
                strassen(T1, B11, local_M[1], half);
                break;
            case 2:
                sub_mat(B12, B22, T2, half);
                strassen(A11, T2, local_M[2], half);
                break;
            case 3:
                sub_mat(B21, B11, T2, half);
                strassen(A22, T2, local_M[3], half);
                break;
            case 4:
                add_mat(A11, A12, T1, half);
                strassen(T1, B22, local_M[4], half);
                break;
            case 5:
                sub_mat(A21, A11, T1, half);
                add_mat(B11, B12, T2, half);
                strassen(T1, T2, local_M[5], half);
                break;
            case 6:
                sub_mat(A12, A22, T1, half);
                add_mat(B21, B22, T2, half);
                strassen(T1, T2, local_M[6], half);
                break;
        }
    }

    int *M[7];
    for (int i = 0; i < 7; ++i)
        M[i] = (rank == 0) ? calloc(half * half, sizeof(int)) : NULL;

    for (int i = 0; i < 7; ++i) {
        MPI_Reduce(local_M[i], M[i], half * half, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    }

    if (rank == 0) {
        int *C11 = C;
        int *C12 = C + half;
        int *C21 = C + n * half;
        int *C22 = C + n * half + half;

        add_mat(M[0], M[3], T1, half);
        sub_mat(T1, M[4], T1, half);
        add_mat(T1, M[6], C11, half);

        add_mat(M[2], M[4], C12, half);
        add_mat(M[1], M[3], C21, half);

        sub_mat(M[0], M[1], T1, half);
        add_mat(T1, M[2], T1, half);
        add_mat(T1, M[5], C22, half);
    }

    free(A11); free(A12); free(A21); free(A22);
    free(B11); free(B12); free(B21); free(B22);
    for (int i = 0; i < 7; ++i) {
        free(local_M[i]);
        if (rank == 0) free(M[i]);
    }
    free(T1); free(T2);
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const int n = MATRIX_SIZE;
    int *A = malloc(n * n * sizeof(int));
    int *B = malloc(n * n * sizeof(int));
    int *C = calloc(n * n, sizeof(int));

    if (rank == 0) {
        srand((unsigned)time(NULL));
        fill_random(A, n * n);
        fill_random(B, n * n);
    }

    MPI_Bcast(A, n * n, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(B, n * n, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    double t0 = MPI_Wtime();

    strassen(A, B, C, n);

    if (rank == 0) {
        double elapsed = MPI_Wtime() - t0;
        printf("Strassen distribuido terminó en %.6f s\n", elapsed);
    }

    free(A); free(B); free(C);
    MPI_Finalize();
    return 0;
}




