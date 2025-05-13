#include <mpi.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#define DATA_SIZE 2097152

int main(int argc, char** argv)
{
    int world_size;
    int world_rank;
    int root = 0;
    int* data;

    MPI_Init(NULL, NULL);

    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    int size = DATA_SIZE;
    if (size <= 0)
    {
        printf("DATA_SIZE tiene valor erróneo: %d, lo igualamos a 1024!!\n", size);
        size = 1024;
    }

    data = (int *)malloc(sizeof(int) * size);
    srand(time(NULL));
    if (world_rank == root) 
    {
        printf("Process 0 broadcasting data of size: %d \n", size);
        for (int i=0; i < size; i++)
        {
            data[i] = rand() % 10 +1;
            if(i < 5)
            {
                printf("[%d]-> %d \n", i , data[i]);
            }
            if(i==5)
                printf("... \n");
            if(size > 10)
            {
                if(i >= size - 3)
                    printf("[%d]-> %d \n", i , data[i]);
            }
        }
    }

    MPI_Bcast(data, size, MPI_INT, root, MPI_COMM_WORLD);

    if (world_rank != root)
    {
        printf("Process %d received data...\n", world_rank);

        for (int i = 0; i < size; i++)
        {
            if(i < 5)
            {
                printf("[%d]-> %d \n", i , data[i]);
            }
            if(i==5)
                printf("... \n");
            if(size > 10)
            {
                if(i >= size - 3)
                    printf("[%d]-> %d \n", i , data[i]);
            }
        }
    }
    free(data);

    MPI_Finalize();
    return 0;
}
