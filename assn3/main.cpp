#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <mpi.h>
#include <unistd.h>
#include <algorithm>

const int MAX_INT_SIZE = 1000;
const int LIST_SIZE = 100;

int * GetUnsortedList(int n){
	int * list = new int[n];
	for(int i = 0; i < n; i++){
		list[i] = rand() % MAX_INT_SIZE;
	}
	return list;
}

void PrintList(int * list){
	for(int i = 0; i < LIST_SIZE; i++){
		std::cout<<list[i]<<",";
	}
	std::cout << std::endl;
}

int * SortList(int * unsorted_list){
	std::sort(unsorted_list, unsorted_list+LIST_SIZE);
	return unsorted_list;
}

int * MergeLists(int ** split_lists){
	return new int;
}

void MasterRoutine(int world_size){
	// Code to run if you're the big boss man
	int * unsorted_list = GetUnsortedList(LIST_SIZE);
	std::cout << "Sorting the following list: " << std::endl;
	PrintList(unsorted_list);
	for(int i = 1; i < world_size; i++){
		MPI_Send(&unsorted_list, LIST_SIZE/(world_size-1), MPI_INT, i, 0, MPI_COMM_WORLD);
		unsorted_list += LIST_SIZE/(world_size-1);
	}
	int ** split_lists = new int *;
	// Wait to receive the split lists
	for(int i = 1; i < world_size; i++){
		MPI_Recv(split_lists[i], LIST_SIZE/(world_size-1), MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}
	int * sorted_list = MergeLists(split_lists);
	std::cout << "Sorted list: " << std::endl;
	PrintList(sorted_list);
}

void SlaveRoutine(int world_size){
	// Code to run if you're not the big boss man
	int * data;
        MPI_Recv(data, LIST_SIZE/(world_size-1), MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	SortList(data);
	MPI_Send(data, LIST_SIZE/(world_size-1), MPI_INT, 0, 0, MPI_COMM_WORLD);
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
		MasterRoutine(world_size);
	} else {
		SlaveRoutine(world_size);
	}
	// Finalize the MPI environment.
	MPI_Finalize();
	return 0;
}

