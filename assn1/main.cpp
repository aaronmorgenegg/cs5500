#include <iostream>
#include <mpi.h>

int main(int argc, char** argv) {
	// initialize MPI
	// MPI_Init(&argc, &argv);
	MPI_Init(&argc, &argv);

	// stores number of processes in world_size
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	// Get the rank of this process
	int world_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

	// Hello World!
	std::cout << "Hello world! I am  " <<  world_rank << " of " << world_size << std::endl;

	// Finalize the MPI environment.
	MPI_Finalize();
	return 0;
}

