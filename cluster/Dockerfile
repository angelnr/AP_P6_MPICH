FROM nlknguyen/alpine-mpich:onbuild

# # ------------------------------------------------------------
# # Build MPI project
# # ------------------------------------------------------------

# Put all build steps and additional package installation here

# Note: the current directory is ${WORKDIR:=/project}, which is
# also the default directory where ${USER:=mpi} will SSH login to

# Copy the content of `project` directory in the host machine to 
# the current working directory in this Docker image
COPY project/ .

# Normal build command
RUN mpicc -o mpi_hello_world mpi_hello_world.c
RUN mpicc -o reduce reduce.c
RUN mpicc -o bcast bcast.c
RUN mpicc -o scatter_gather scatter_gather.c
RUN mpicc -o send_recv send_recv.c
RUN mpicc -o block_rows_algorithm block_rows_algorithm.c
RUN mpicc -o cannons_algorithm cannons_algorithm.c
RUN mpicc -o foxs_algorithm foxs_algorithm.c
RUN mpicc -o foxs_algorithm foxs_algorithm.c
RUN mpicc -o strassens_algorithm strassens_algorithm.c

# ####################
# For Docker beginner:

# After Docker syntax `RUN`, you can execute any command available in 
# the current shell of the image.

# To switch to root:    USER root
# To switch back to default user: USER ${USER}
