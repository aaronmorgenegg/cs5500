#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <mpi.h>
#include <unistd.h>
#include <algorithm>

void print1per(int data, int rank, int size){ // Print 1 per function from class
	int *dArray = new int [size];
	MPI_Gather(&data,1,MPI_INT,dArray,1,MPI_INT,0,MCW);
	if(rank==0){
		for(int i=0;i<size;++i){
			cout<<i<<":";
			cout<<dArray[i]<<" ";
		}
		cout<<endl;
	}
	return;
}

int cube(int i, int sendData){ // Cube function from class
	int rank;
	int size;
	MPI_Comm_rank(MCW, &rank); 
	MPI_Comm_size(MCW, &size); 
	int dest;
	int recvData;
	int mask=1;
	mask = mask << i;
	dest = rank ^ mask;
	MPI_Send(&sendData,1,MPI_INT,dest,0,MCW);
	MPI_Recv(&recvData,1,MPI_INT,dest,0,MCW,MPI_STATUS_IGNORE);

	return recvData;
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

	// Finalize the MPI environment.
	MPI_Finalize();
	return 0;
}

