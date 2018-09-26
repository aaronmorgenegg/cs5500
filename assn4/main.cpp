#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <mpi.h>
#include <unistd.h>
#include <algorithm>

int * getBitonicList(int size){ // Create a bitonic list of given size
	// CAUTION : designed to work with size=power of 2
	int * list = new int[size];
	int value = size/2;
	for(int i = 0; i < size; i++){
		list[i] = value;
		if(value == 1){
			value = size;
		} else{
			value--;
		}
	}
	return list;
}

int * printList(int * list, int size){
	for(int i=0;i<size;++i){
		std::cout<<i<<":";
		std::cout<<list[i]<<" ";
	}
	std::cout<<std::endl;
}

void print1per(int data, int rank, int size){ // Print 1 per function from class
	int *dArray = new int [size];
	MPI_Gather(&data,1,MPI_INT,dArray,1,MPI_INT,0,MPI_COMM_WORLD);
	if(rank==0){
		printList(darray, size);
	}
	return;
}

int cube(int i, int sendData){ // Cube function from class
	int rank;
	int size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); 
	MPI_Comm_size(MPI_COMM_WORLD, &size); 
	int dest;
	int recvData;
	int mask=1;
	mask = mask << i;
	dest = rank ^ mask;
	MPI_Send(&sendData,1,MPI_INT,dest,0,MPI_COMM_WORLD);
	MPI_Recv(&recvData,1,MPI_INT,dest,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);

	return recvData;
}

int main(int argc, char** argv) {
	MPI_Init(&argc, &argv);
	srand(time(NULL));
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	int world_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

	int * list = getBitonicList(8);
	printList(list, 8);

	MPI_Finalize();
	return 0;
}

