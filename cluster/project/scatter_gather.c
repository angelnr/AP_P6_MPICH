

/*
Ejemplo de gather y scatter con MPICH.
descripción de lo que hacen en mpitutorial.com: https://mpitutorial.com/tutorials/mpi-scatter-gather-and-allgather/
cabeceras en la documentación oficial: https://www.mpich.org/static/docs/v4.0.3/www3/MPI_Scatter.html
https://www.mpich.org/static/docs/v4.0.3/www3/MPI_Gather.html

el nodo 0 va a repartir un buffer de números del 1 al 10 a cada nodo del clúster,
cada nodo los va a multiplicar por 2, y los vamos a recoger. 
*/



#include <mpi.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#define DATA_SIZE 40

int main(int argc, char** argv)
{
    int world_size;
    int world_rank;
    int root = 0;
    int* data = NULL;
    int* recv_data = NULL;

    MPI_Init(NULL, NULL);

    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    int size = DATA_SIZE;
    if (size <= 0)
    {
        printf("DATA_SIZE tiene valor erróneo: %d, lo igualamos a 1024!!\n", size);
        size = 1024;
    }

    if ( size % world_size != 0)
    {
        printf("El tamaño del vector: %d, no es divisible por el número de nodos: %d \n", size, world_size);
        return -1;
    }

    int each_node_size = size / world_size;

    recv_data = (int *)malloc(sizeof(int) * (each_node_size));
    srand(time(NULL));
    if (world_rank == root) 
    {
        data = (int *)malloc(sizeof(int) * size);
        printf("Process 0 scattering data of size: %d \n", size);
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
    MPI_Scatter(data, each_node_size, MPI_INT, recv_data,
        each_node_size, MPI_INT, root, MPI_COMM_WORLD);

    printf("Process %d received data...\n", world_rank);
    for (int i = 0; i < each_node_size; i++)
    {
        if(i == 0)
            printf("Nodo: %d, data[%d]-> %d \n",world_rank, i , recv_data[i]);
        
        recv_data[i] += recv_data[i];
        
        if(i == 0)
            printf("Nodo: %d, convertimos datos, data[%d]-> %d \n",world_rank, i , recv_data[i]);        
    }
    MPI_Gather(recv_data, each_node_size, MPI_INT, data,
        each_node_size, MPI_INT, root, MPI_COMM_WORLD);
    if (world_rank == root) 
    {
        printf("Process 0 after gathering, getting data:\n");
        for (int i=0; i < size; i++)
        {
            if(i < 5)
                printf("[%d]-> %d \n", i , data[i]);
            if(i==5)
                printf("... \n");
            if(size > 10)
            {
                if(i >= size - 3)
                    printf("[%d]-> %d \n", i , data[i]);
            }
        }
        if (data != NULL)
            free(data);
    }
    if(recv_data != NULL)
        free(recv_data);

    MPI_Finalize();
    return 0;
}
