#include <iostream>
#include <fstream>
#include <time.h>
#include <stdlib.h>
#include <mpi.h>
#include <unistd.h>
#include <algorithm>
#include <math.h>
#include <string>
#include <vector>

const int RESOLUTION = 1024; // size of array, 2d so it's squared
const bool PROPORTION = 20; // % of squares initially alive

std::vector<bool> getEmptyArray(int size){
	// 2d array represented by 1d array
	std::vector<bool> plot(size*size,false);
        return plot;
}

std::vector<bool> initArray(int size, int proportion){
	// Return an array of size x size, with a proportion% chance of being alive
	std::vector<bool> plot = getEmptyArray(size);
	for(int i = 0; i < size*size; i++){
		if(rand()% 100 < proportion){
			plot[i] = true;
		}
	}
	return plot;
}

std::vector<bool> gatherArray(world_rank){
	if(world_rank==0){
		std::vector<bool> recv_data = getEmptyArray(RESOLUTION);
		MPI_Gather(&plot[start*RESOLUTION],RESOLUTION*offset,MPI_BOOL,&recv_data.front(),RESOLUTION*offset,MPI_BOOL,0,MPI_COMM_WORLD);
		plotImage(recv_data);
	} else{
		MPI_Gather(&plot[start*RESOLUTION],RESOLUTION*offset,MPI_BOOL,NULL,RESOLUTION*offset,MPI_BOOL,0,MPI_COMM_WORLD);
	}
}

void updateCell(int index, std::vector<bool> plot){
	bool value = plot[index];
	int neighbours = countNeighbours(index, plot);
	if(value == false){
		if(neighbours == 3){
			plot[index] = true;
		}
	} else{
		if(neighbours < 2){
			plot[index] = false;
		}
		if(neighbours > 3){
			plot[index] = false;
		}
	}
}

void gameOfLife(int world_rank, int world_size){
	
}

int main(int argc, char** argv) {
	MPI_Init(&argc, &argv);
	srand(time(NULL));
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	int world_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

	gameOfLife(world_rank, world_size);
	MPI_Finalize();
	return 0;
}

