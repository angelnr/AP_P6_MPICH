#include <mpi.h>
#include <stdio.h>


int main(int argc, char** argv)
{
    MPI_Init(NULL, NULL);

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    int number;
    if (world_rank == 0)
    {
        number = 100;
        MPI_Send(&number, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
    } 
    else if (world_rank == 1) 
    {
        MPI_Recv(&number, 1, MPI_INT, 0, 0, MPI_COMM_WORLD,
             MPI_STATUS_IGNORE);
        printf("Process 1 received number %d from process 0\n",
           number);
    }
    else if (world_rank == 2) 
    {
        number = 200;
        MPI_Send(&number, 1, MPI_INT, 3, 0, MPI_COMM_WORLD);
    } 
    else if (world_rank == 3) 
    {
        MPI_Recv(&number, 1, MPI_INT, 2, 0, MPI_COMM_WORLD,
            MPI_STATUS_IGNORE);
        printf("Process 3 received number %d from process 2\n",
            number);
    } 
    MPI_Finalize();
    return 0;
}