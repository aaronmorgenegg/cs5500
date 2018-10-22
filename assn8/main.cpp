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
const int DAYS = 100;

std::vector<bool> getEmptyArray(int size){
	// 2d array represented by 1d array
	std::vector<bool> plot(RESOLUTION*size,false);
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

void sendPlot(std::vector<bool> plot, int world_size){	
	for(int i = 1; i < world_size; i++){
		MPI_Send(&plot[0],RESOLUTION*RESOLUTION,MPI_BOOL,i,0,MPI_COMM_WORLD);	
	}
}

std::vector<bool> gatherArray(int world_rank, int world_size, std::vector<bool> plot){
	int offset = RESOLUTION/world_size;
	if(world_rank==0){
		std::vector<bool> recv_data = getEmptyArray(RESOLUTION);
		MPI_Gather(&sub_plot[0],RESOLUTION*offset,MPI_BOOL,&recv_data.front(),RESOLUTION*offset,MPI_BOOL,0,MPI_COMM_WORLD);
		sendPlot(recv_data, world_size);
		return recv_data;
	} else{
		MPI_Gather(&sub_plot[0],RESOLUTION*offset,MPI_BOOL,NULL,RESOLUTION*offset,MPI_BOOL,0,MPI_COMM_WORLD);
		MPI_Recv(&plot[0],RESOLUTION*RESOLUTION,MPI_BOOL,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
	}
	return plot;
}

int countNeighbours(int index, std::vector<bool> plot){
	int count = 0;
	int i = index / RESOLUTION;
	int j = index % RESOLUTION;
	if(j > 1 && i > 1 && plot[i*(RESOLUTION-1)+j-1]) count++;
	if(i > 1 && plot[i*(RESOLUTION-1)+j]) count++;
	if(j < RESOLUTION-2 && i > 1 && plot[i*(RESOLUTION-1)+j+1]) count++;
	if(j > 1 && plot[i*(RESOLUTION)+j-1]) count++;
	if(j < RESOLUTION-2 && plot[i*(RESOLUTION)+j+1]) count++;
	if(j > 1 && i < RESOLUTION-2 && plot[i*(RESOLUTION+1)+j-1]) count++;
	if(i < RESOLUTION-2 && plot[i*(RESOLUTION+1)+j]) count++;
	if(j < RESOLUTION-2 && i < RESOLUTION-2 && plot[i*(RESOLUTION+1)+j+1]) count++;
	return count;
}

bool updateCell(int index, std::vector<bool> plot){
	bool value = plot[index];
	int neighbours = countNeighbours(index, plot);
	if(value == false){
		if(neighbours == 3){
			return true;
		}
	} else{
		if(neighbours < 2){
			return false;
		}
		if(neighbours > 3){
			return false;
		}
	}
	return value;
}

void updateSubPlot(std::vector<bool> plot, int start_index, int chunk_size){
	std::vector<bool> sub_plot = getEmptyArray(chunk_size);
	for(int i = 0; i < chunk_size; i++){
		sub_plot[i] = updateCell(i+start_index, plot);
	}
	return sub_plot;
}

std::vector<bool> setupPlot(world_rank, world_size){
	if(world_rank == 0){
		std::vector<bool> plot = initArray(RESOLUTION, PROPORTION);
		sendPlot(plot, world_size);
	} else{
		std::vector<bool> plot = getEmptyArray(RESOLUTION);
		MPI_Recv(&plot[0],RESOLUTION*RESOLUTION,MPI_BOOL,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
	}
	return plot;
}

void gameOfLife(int world_rank, int world_size){
	std::vector<bool> plot = setupPlot(world_rank, world_size);
	for(int i = 0; i < DAYS; i++){
		// divide work between processes
		int chunk_size = RESOLUTION*RESOLUTION/world_size;
		int start_index = world_rank * chunk_size;
		// update your portion of the array
		std::vector<bool> sub_plot = updateSubPlot(plot, start_index, chunk_size);
		// merge portions back into array for next day
		plot = gatherArray(world_rank, world_size, plot, sub_plot);
	}	
}

int main(int argc, char** argv) {
	MPI_Init(&argc, &argv);
	srand(time(NULL));
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	int world_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	// MPI_Send(&sendData,1,MPI_INT,dest,0,MPI_COMM_WORLD);
        // MPI_Recv(&recvData,1,MPI_INT,dest,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);

	gameOfLife(world_rank, world_size);
	MPI_Finalize();
	return 0;
}

