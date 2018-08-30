#include <iostream>
#include <mpi.h>

int main(int argc, char** argv) {
	// initialize MPI
	MPI_Init(&argc, &argv);

	// stores number of processes in world_size
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	// Get the rank of this process
	int world_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

	int data = world_rank;
	MPI_Send(
		&data, // data to send
		1, // count, or number of things passed
		MPI_INT, // datatype
		(world_rank+1)%world_size, // destination
		0, // tag of message
		MPI_COMM_WORLD // MPI communicator
	);

	MPI_Recv(
		&data, // data to recieve
		1, // count, or number of things passed
	    	MPI_INT, // datatype
		MPI_ANY_SOURCE, // source
		MPI_ANY_TAG, // tag of message
		MPI_COMM_WORLD, // MPI communicator
		MPI_STATUS_IGNORE // MPI status
	);

	std::cout << "I am " << world_rank << " of " << world_size << ", recieving a message from number " << data << std::endl;

	// Finalize the MPI environment.
	MPI_Finalize();
	return 0;
}

