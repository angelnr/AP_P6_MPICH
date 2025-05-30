#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

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

    // Verificar que el número de procesos sea cuadrado perfecto
    int q = (int)sqrt(size);
    if (q * q != size) {
        if (rank == 0) fprintf(stderr, "Error: Número de procesos (%d) debe ser cuadrado perfecto (ej. 4, 9, 16).\n", size);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    
    int n = MATRIX_SIZE;
    int block_size = n / q;  // tamaño de bloque local
    if (n % q != 0) {
        if (rank == 0) fprintf(stderr, "Error: Tamaño de matriz (%d) debe ser divisible por q (%d)\n", n, q);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    // Crear topología de malla 2D (toroidal)
    int dims[2] = {q, q};
    int periods[2] = {1, 1};  // 1 = conexiones cíclicas (toroidal)
    MPI_Comm grid_comm;
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 0, &grid_comm);
    
    // Obtener coordenadas y vecinos
    int coords[2];
    MPI_Cart_coords(grid_comm, rank, 2, coords);
    int left, right, up, down;
    MPI_Cart_shift(grid_comm, 1, 1, &left, &right);  // Eje x (columnas)
    MPI_Cart_shift(grid_comm, 0, 1, &up, &down);      // Eje y (filas)

    // Reservar memoria para bloques locales
    int *Ablock = malloc(block_size * block_size * sizeof(int));
    int *Bblock = malloc(block_size * block_size * sizeof(int));
    int *Cblock = calloc(block_size * block_size, sizeof(int));

    // Matrices globales solo en root
    int *A_global = NULL, *B_global = NULL, *C_global = NULL;
    if (rank == 0) {
        A_global = malloc(n * n * sizeof(int));
        B_global = malloc(n * n * sizeof(int));
        C_global = calloc(n * n, sizeof(int));
        srand(time(NULL));
        fill_random(A_global, n * n);
        fill_random(B_global, n * n);
    }

    // Crear tipo de dato para bloques (no contiguos en matriz global)
    MPI_Datatype block_type;
    MPI_Type_vector(block_size, block_size, n, MPI_INT, &block_type);
    MPI_Type_commit(&block_type);

    // Distribuir bloques con desplazamiento inicial (skew)
    int *sendcounts = NULL;
    int *displs = NULL;
    
    if (rank == 0) {
        sendcounts = malloc(size * sizeof(int));
        displs = malloc(size * sizeof(int));
        for (int i = 0; i < size; i++) sendcounts[i] = 1;
        
        // Calcular desplazamientos para distribución skew
        for (int i = 0; i < q; i++) {
            for (int j = 0; j < q; j++) {
                int proc;
                MPI_Cart_rank(grid_comm, (int[]){i, j}, &proc);
                
                // Desplazamiento inicial para A: fila i, columna (j + i) mod q
                int skew_col_A = (j + i) % q;
                displs[proc] = i * block_size * n + skew_col_A * block_size;
                
                // Desplazamiento inicial para B: fila (i + j) mod q, columna j
                int skew_row_B = (i + j) % q;
                displs[proc] = skew_row_B * block_size * n + j * block_size;
            }
        }
    }

    // Distribuir bloques de A y B
    MPI_Scatterv(A_global, sendcounts, displs, block_type, 
                Ablock, block_size * block_size, MPI_INT, 
                0, grid_comm);
                
    MPI_Scatterv(B_global, sendcounts, displs, block_type, 
                Bblock, block_size * block_size, MPI_INT, 
                0, grid_comm);

    // Sincronizar y comenzar temporización
    MPI_Barrier(grid_comm);
    double t0 = MPI_Wtime();

    // Algoritmo de Cannon (q pasos)
    for (int step = 0; step < q; ++step) {
        // Multiplicación local: Cblock += Ablock * Bblock
        for (int i = 0; i < block_size; ++i) {
            for (int k = 0; k < block_size; ++k) {
                int a_val = Ablock[i * block_size + k];
                for (int j = 0; j < block_size; ++j) {
                    Cblock[i * block_size + j] += a_val * Bblock[k * block_size + j];
                }
            }
        }

        // Rotar bloques: A hacia izquierda, B hacia arriba
        MPI_Sendrecv_replace(Ablock, block_size * block_size, MPI_INT, 
                            left, 0, right, 0, grid_comm, MPI_STATUS_IGNORE);
                            
        MPI_Sendrecv_replace(Bblock, block_size * block_size, MPI_INT, 
                            up, 0, down, 0, grid_comm, MPI_STATUS_IGNORE);
    }

    // Detener tiempo y recolectar resultados
    double local_elapsed = MPI_Wtime() - t0;
    double max_elapsed;
    MPI_Reduce(&local_elapsed, &max_elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, grid_comm);

    // Recolectar bloques de resultado
    MPI_Gatherv(Cblock, block_size * block_size, MPI_INT,
               C_global, sendcounts, displs, block_type,
               0, grid_comm);

    // Reportar resultados
    if (rank == 0) {
        printf("Cannon completado en %.6f segundos\n", max_elapsed);
        
        // Verificación opcional (solo para matrices pequeñas)
        if (n <= 8) {
            printf("Resultado C[0:3][0:3]:\n");
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    printf("%d ", C_global[i * n + j]);
                }
                printf("\n");
            }
        }
        
        free(A_global);
        free(B_global);
        free(C_global);
        free(sendcounts);
        free(displs);
    }

    // Liberar recursos
    MPI_Type_free(&block_type);
    free(Ablock);
    free(Bblock);
    free(Cblock);
    MPI_Comm_free(&grid_comm);
    MPI_Finalize();
    return 0;
}