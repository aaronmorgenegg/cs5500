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

void PrintList(int * list, int list_length){
	for(int i = 0; i < list_length; i++){
		std::cout<<list[i]<<",";
	}
	std::cout << std::endl;
}

int * SortList(int * unsorted_list, int list_length){
	std::sort(unsorted_list, unsorted_list+list_length);
	return unsorted_list;
}

int * MergeLists(int ** split_lists, int world_size){
	int * merged_list = new int[LIST_SIZE];
	for(int i = 0; i < LIST_SIZE; i++){
		int min_index = 0;
		for(int j = 1; j < world_size; j++){
			if(split_lists[j][0] < split_lists[min_index][0]){
				min_index = j;
			}
		}
		merged_list[i] = split_lists[min_index][0];
		split_lists[min_index]+=1;
	}
	return merged_list;
}

int * CreateAndPrintList(int world_size){
	int * unsorted_list = GetUnsortedList(LIST_SIZE);
	std::cout << "Sorting the following list: " << std::endl;
	PrintList(unsorted_list, LIST_SIZE);
	return unsorted_list;
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

	int sub_length = LIST_SIZE/(world_size);

	int * unsorted_list;
	if(world_rank == 0){
		unsorted_list = CreateAndPrintList(world_size);
	}
	int * sub_list = new int[sub_length];
	MPI_Scatter(unsorted_list, sub_length, MPI_INT, sub_list, sub_length, MPI_INT, 0, MPI_COMM_WORLD);

	SortList(sub_list, sub_length);
	MPI_Send(sub_list, sub_length, MPI_INT, 0, 0, MPI_COMM_WORLD);

	if(world_rank == 0){
		int ** sub_lists = new int*[world_size];
		for(int i = 0; i < world_size; i++){
			sub_lists[i] = new int[sub_length];
			MPI_Recv(sub_lists[i], sub_length, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
		int * sorted_list = MergeLists(sub_lists, world_size);
		std::cout << "Sorted List: " << std::endl;
		PrintList(sorted_list, LIST_SIZE);
	}

	// Finalize the MPI environment.
	MPI_Finalize();
	return 0;
}

