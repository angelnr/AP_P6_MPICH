/* strassen_mpi_fixed.c
 * Versión rápida que evita punteros NULL en los ranks ≠ 0.
 * Cada proceso mantiene copias completas de A, B y C.
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define MATRIX_SIZE 1024  /* múltiplo de 2                    */
#define MAX_VAL 10
#define MIN_VAL 1
#define THRESHOLD 256      /* por debajo: multiplicación clásica */

/* ---------- utilidades básicas ---------- */
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
    /* copia sub-matriz n/2 × n/2 que empieza en (r,c) */
    int half = n / 2;
    for (int i = 0; i < half; ++i)
        for (int j = 0; j < half; ++j)
            S[i * half + j] = M[(i + r) * n + (j + c)];
}

/* -------------- Strassen recursivo --------------- */
void strassen(int *A, int *B, int *C, int n) {
    if (n <= THRESHOLD) {
        classic_multiply(A, B, C, n);
        return;
    }

    int half = n / 2;
    size_t sz = half * half * sizeof(int);

    /* reservar sub-bloques */
    int *A11 = malloc(sz), *A12 = malloc(sz), *A21 = malloc(sz), *A22 = malloc(sz);
    int *B11 = malloc(sz), *B12 = malloc(sz), *B21 = malloc(sz), *B22 = malloc(sz);
    int *M1  = calloc(half * half, sizeof(int));
    int *M2  = calloc(half * half, sizeof(int));
    int *M3  = calloc(half * half, sizeof(int));
    int *M4  = calloc(half * half, sizeof(int));
    int *M5  = calloc(half * half, sizeof(int));
    int *M6  = calloc(half * half, sizeof(int));
    int *M7  = calloc(half * half, sizeof(int));
    int *T1  = malloc(sz), *T2 = malloc(sz);

    split(A, A11, n, 0,     0);
    split(A, A12, n, 0,     half);
    split(A, A21, n, half,  0);
    split(A, A22, n, half,  half);
    split(B, B11, n, 0,     0);
    split(B, B12, n, 0,     half);
    split(B, B21, n, half,  0);
    split(B, B22, n, half,  half);

    /* M1 = (A11 + A22)*(B11 + B22) */
    add_mat(A11, A22, T1, half);
    add_mat(B11, B22, T2, half);
    strassen(T1, T2, M1, half);

    /* M2 = (A21 + A22) * B11 */
    add_mat(A21, A22, T1, half);
    strassen(T1, B11, M2, half);

    /* M3 = A11 * (B12 − B22) */
    sub_mat(B12, B22, T2, half);
    strassen(A11, T2, M3, half);

    /* M4 = A22 * (B21 − B11) */
    sub_mat(B21, B11, T2, half);
    strassen(A22, T2, M4, half);

    /* M5 = (A11 + A12) * B22 */
    add_mat(A11, A12, T1, half);
    strassen(T1, B22, M5, half);

    /* M6 = (A21 − A11) * (B11 + B12) */
    sub_mat(A21, A11, T1, half);
    add_mat(B11, B12, T2, half);
    strassen(T1, T2, M6, half);

    /* M7 = (A12 − A22) * (B21 + B22) */
    sub_mat(A12, A22, T1, half);
    add_mat(B21, B22, T2, half);
    strassen(T1, T2, M7, half);

    /* recomponer C */
    int *C11 = C;
    int *C12 = C + half;
    int *C21 = C + n * half;
    int *C22 = C + n * half + half;

    /* C11 = M1 + M4 − M5 + M7 */
    add_mat(M1, M4, T1, half);
    sub_mat(T1, M5, T1, half);
    add_mat(T1, M7, C11, half);

    /* C12 = M3 + M5 */
    add_mat(M3, M5, C12, half);

    /* C21 = M2 + M4 */
    add_mat(M2, M4, C21, half);

    /* C22 = M1 − M2 + M3 + M6 */
    sub_mat(M1, M2, T1, half);
    add_mat(T1, M3, T1, half);
    add_mat(T1, M6, C22, half);

    /* liberar */
    free(A11); free(A12); free(A21); free(A22);
    free(B11); free(B12); free(B21); free(B22);
    free(M1);  free(M2);  free(M3);  free(M4);
    free(M5);  free(M6);  free(M7);
    free(T1);  free(T2);
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const int n = MATRIX_SIZE;

    /* 1. reservar A,B,C en todos los procesos */
    int *A = malloc(n * n * sizeof(int));
    int *B = malloc(n * n * sizeof(int));
    int *C = calloc(n * n, sizeof(int));

    if (rank == 0) {
        srand((unsigned)time(NULL));
        fill_random(A, n * n);
        fill_random(B, n * n);
    }

    /* 2. difundir A y B completas */
    MPI_Bcast(A, n * n, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(B, n * n, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    double t0 = MPI_Wtime();


    /* ====  A Q U Í  va la llamada condicional ======================= */
    if (rank == 0)
    strassen(A, B, C, n);  /* solo el root */
    MPI_Barrier(MPI_COMM_WORLD);    


    /*
     3. cada rank ejecuta Strassen (sin reparto real) 
    strassen(A, B, C, n);

    */

    double elapsed = MPI_Wtime() - t0;
    double max_time;
    MPI_Reduce(&elapsed, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    if (rank == 0)
        printf("Strassen (copia completa en cada nodo) terminó en %.6f s\n",
               max_time);

    free(A); free(B); free(C);
    MPI_Finalize();
    return 0;
}
