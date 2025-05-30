#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MATRIX_SIZE 1024  // tamaño de la matriz (debe ser múltiplo de 2)
#define MAX_VAL 10
#define MIN_VAL 1
#define THRESHOLD 32  // Umbral para multiplicación clásica

// Función para llenar la matriz con valores aleatorios
void fill_random(int *mat, int elements) {
    for (int i = 0; i < elements; ++i)
        mat[i] = rand() % (MAX_VAL - MIN_VAL + 1) + MIN_VAL;
}

// Multiplicación clásica de matrices
void classic_multiply(int *A, int *B, int *C, int n) {
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            for (int k = 0; k < n; ++k)
                C[i * n + j] += A[i * n + k] * B[k * n + j];
}

// Función para dividir una matriz en submatrices 2x2
void split_matrix(int *mat, int *submat, int n, int row_start, int col_start) {
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            submat[i * n + j] = mat[(i + row_start) * n + (j + col_start)];
}

// Función para sumar dos matrices
void add_matrix(int *A, int *B, int *C, int n) {
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            C[i * n + j] = A[i * n + j] + B[i * n + j];
}

// Función para restar dos matrices
void subtract_matrix(int *A, int *B, int *C, int n) {
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            C[i * n + j] = A[i * n + j] - B[i * n + j];
}

// Función para calcular las 7 multiplicaciones de Strassen
void strassen_multiply(int *A, int *B, int *C, int n, int rank, int size) {
    if (n <= THRESHOLD) {
        // Si el tamaño de la matriz es pequeño, hacemos multiplicación clásica
        classic_multiply(A, B, C, n);
        return;
    }

    // Dividir las matrices A y B en 4 submatrices
    int *A11 = malloc((n / 2) * (n / 2) * sizeof(int));
    int *A12 = malloc((n / 2) * (n / 2) * sizeof(int));
    int *A21 = malloc((n / 2) * (n / 2) * sizeof(int));
    int *A22 = malloc((n / 2) * (n / 2) * sizeof(int));
    int *B11 = malloc((n / 2) * (n / 2) * sizeof(int));
    int *B12 = malloc((n / 2) * (n / 2) * sizeof(int));
    int *B21 = malloc((n / 2) * (n / 2) * sizeof(int));
    int *B22 = malloc((n / 2) * (n / 2) * sizeof(int));

    // Dividir las matrices A y B en submatrices 2x2
    split_matrix(A, A11, n, 0, 0);
    split_matrix(A, A12, n, 0, n / 2);
    split_matrix(A, A21, n, n / 2, 0);
    split_matrix(A, A22, n, n / 2, n / 2);
    split_matrix(B, B11, n, 0, 0);
    split_matrix(B, B12, n, 0, n / 2);
    split_matrix(B, B21, n, n / 2, 0);
    split_matrix(B, B22, n, n / 2, n / 2);

    // Crear las 7 matrices auxiliares (M1 a M7)
    int *M1 = malloc((n / 2) * (n / 2) * sizeof(int));
    int *M2 = malloc((n / 2) * (n / 2) * sizeof(int));
    int *M3 = malloc((n / 2) * (n / 2) * sizeof(int));
    int *M4 = malloc((n / 2) * (n / 2) * sizeof(int));
    int *M5 = malloc((n / 2) * (n / 2) * sizeof(int));
    int *M6 = malloc((n / 2) * (n / 2) * sizeof(int));
    int *M7 = malloc((n / 2) * (n / 2) * sizeof(int));

    // Calcular los 7 productos de Strassen
    // M1 = (A11 + A22) * (B11 + B22)
    int *temp1 = malloc((n / 2) * (n / 2) * sizeof(int));
    int *temp2 = malloc((n / 2) * (n / 2) * sizeof(int));
    add_matrix(A11, A22, temp1, n / 2);
    add_matrix(B11, B22, temp2, n / 2);
    strassen_multiply(temp1, temp2, M1, n / 2, rank, size);

    // M2 = (A21 + A22) * B11
    add_matrix(A21, A22, temp1, n / 2);
    strassen_multiply(temp1, B11, M2, n / 2, rank, size);

    // M3 = A11 * (B12 - B22)
    subtract_matrix(B12, B22, temp2, n / 2);
    strassen_multiply(A11, temp2, M3, n / 2, rank, size);

    // M4 = A22 * (B21 - B11)
    subtract_matrix(B21, B11, temp2, n / 2);
    strassen_multiply(A22, temp2, M4, n / 2, rank, size);

    // M5 = (A11 + A12) * B22
    add_matrix(A11, A12, temp1, n / 2);
    strassen_multiply(temp1, B22, M5, n / 2, rank, size);

    // M6 = (A21 - A11) * (B11 + B12)
    subtract_matrix(A21, A11, temp1, n / 2);
    add_matrix(B11, B12, temp2, n / 2);
    strassen_multiply(temp1, temp2, M6, n / 2, rank, size);

    // M7 = (A12 - A22) * (B21 + B22)
    subtract_matrix(A12, A22, temp1, n / 2);
    add_matrix(B21, B22, temp2, n / 2);
    strassen_multiply(temp1, temp2, M7, n / 2, rank, size);

    // Recomponer la matriz C a partir de las 7 multiplicaciones
    // C11 = M1 + M4 - M5 + M7
    add_matrix(M1, M4, temp1, n / 2);
    subtract_matrix(temp1, M5, temp1, n / 2);
    add_matrix(temp1, M7, C, n / 2);

    // C12 = M3 + M5
    add_matrix(M3, M5, C + (n / 2), n / 2);

    // C21 = M2 + M4
    add_matrix(M2, M4, C + n * (n / 2), n / 2);

    // C22 = M1 - M2 + M3 + M6
    subtract_matrix(M1, M2, temp1, n / 2);
    add_matrix(temp1, M3, temp1, n / 2);
    add_matrix(temp1, M6, C + n * n / 2, n / 2);

    // Liberar memoria
    free(A11); free(A12); free(A21); free(A22);
    free(B11); free(B12); free(B21); free(B22);
    free(M1); free(M2); free(M3); free(M4); free(M5); free(M6); free(M7);
    free(temp1); free(temp2);
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int n = MATRIX_SIZE;
    int *A = NULL, *B = NULL, *C = NULL;

    // Solo el proceso root inicializa A y B
    if (rank == 0) {
        A = malloc(n * n * sizeof(int));
        B = malloc(n * n * sizeof(int));
        C = malloc(n * n * sizeof(int));
        fill_random(A, n * n);
        fill_random(B, n * n);
    }

    // Llamada a la multiplicación Strassen
    strassen_multiply(A, B, C, n, rank, size);

    if (rank == 0) {
        printf("Multiplicación completada con Strassen distribuido\n");
        // Liberar matrices
        free(A); free(B); free(C);
    }

    MPI_Finalize();
    return 0;
}