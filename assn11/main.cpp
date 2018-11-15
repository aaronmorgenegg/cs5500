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
#define MPI_COMM_WORLD MCW
#define MPI_ANY_SOURCE ANY

// ----- CONSTANTS -----

std::string FILE_SHAKESPEARE = "sample_strings/shakespeare.txt"; // Complete works of shakespeare
std::string FILE_DNA1 = "sample_strings/dna1.fasta"; // Dna sequence
int BINARY_STRING_LENGTH = 10000000; // Length of binary strings for study
int MIN_M = 1; // Smallest length of pattern for study
int MAX_M = 100000; // Largest length of pattern for study
int NUM_CHARS = 256; // number of characters in the alphabet used

// ----- UTILITIES -----



// ----- ALGORITHMS -----

std::vector<int> stringMatchingNaive(std::string text, std::string pattern){
	std::vector<int> found_indices;
	int n = text.length();
	int m = pattern.length();
	for(int i = 0; i < n-m+1; i++){
		for(int j = 0; j < m; j++){
			if(text[i+j] != pattern[j]{
				break;
			}
			if(j == m-1){
				found_indices.push_back(i)
			}
		}
	}
	return found_indices;
}

// ----- MAIN -----

int main(int argc, char** argv) {
	MPI_Init(&argc, &argv);
	srand(time(NULL));
	int world_size;
	MPI_Comm_size(MCW, &world_size);
	int world_rank;
	MPI_Comm_rank(MCW, &world_rank);
	// MPI_Send(&sendData,1,MPI_INT,dest,0,MPI_COMM_WORLD);
        // MPI_Recv(&recvData,1,MPI_INT,dest,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);

	MPI_Finalize();
	return 0;
}

