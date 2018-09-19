#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <mpi.h>
#include <unistd.h>

void MasterRoutine(){
	// Code to run if you're the big boss man
	
}

void SlaveRoutine(){
	// Code to run if you're not the big boss man
	
}

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

	if(world_rank == 0){
		MasterRoutine();
	} else {
		SlaveRoutine();
	}

	// Finalize the MPI environment.
	MPI_Finalize();
	return 0;
}

