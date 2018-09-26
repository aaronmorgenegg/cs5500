#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <mpi.h>
#include <unistd.h>
#include <algorithm>

int main(int argc, char** argv) {
	// initialize MPI
	MPI_Init(&argc, &argv);

	// Initialize rng
	srand(time(NULL));

	// stores number of processes in world_size
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	// Get the rank of this process
	int world_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

	// Finalize the MPI environment.
	MPI_Finalize();
	return 0;
}

