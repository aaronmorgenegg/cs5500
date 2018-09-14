#include <iostream>
#include <mpi.h>

int main(int argc, char** argv) {
	MPI_Init(&argc, &argv);

	int size;
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	// Get the rank of this process
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	int data = 0;
	if(rank==0)data=42;

	MPI_Bcast(&data, 1, MPI_INT, 0, MPI_COMM_WORLD);

	std::cout<<"data:"<<data<<std::endl;

	MPI_Finalize();
	return 0;
}

