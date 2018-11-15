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
#define MCW MPI_COMM_WORLD
#define ANY MPI_ANY_SOURCE

// ----- CONSTANTS -----

std::string FILE_SHAKESPEARE = "sample_strings/shakespeare.txt"; // Complete works of shakespeare
std::string FILE_DNA1 = "sample_strings/dna1.fasta"; // Dna sequence
int BINARY_STRING_LENGTH = 10000000; // Length of binary strings for study
int MIN_M = 1; // Smallest length of pattern for study
int MAX_M = 100000; // Largest length of pattern for study
int NUM_CHARS = 256; // number of characters in the alphabet used

// ----- UTILITIES -----

std::string generateBinaryString(int length, double regularity){
	// Returns a string of 0's and 1's, with given length and regularity ratio(value between 0 and 1)
	int last_char = rand()%2;
	std::string binary_string = std::to_string(last_char);
	for(int i = 0; i < length; i++){
		double p = (double)rand()/(double)RAND_MAX;
		if(p > regularity){
			if(last_char == 0){
				last_char = 1;
			} else{
				last_char = 0;
			}
		}
		binary_string.push_back(std::to_string(last_char)[0]);
	}
	return binary_string;
}

std::vector<int> computeLPSArray(std::string pattern, int m){
	// Computes the prefix/suffix array for KMP algorithm
	std::vector<int> lps(m, 0);
	int length = 0;
	int i = 1;
	while(i < m){
		if(pattern[i]==pattern[length]){
			length++;
			lps[i] = length;
			i++;
		} else{
			if(length != 0){
				length = lps[length-1];
			} else{
				lps[i] = 0;
				i++;
			}
		}
	}
	return lps;
}

std::vector<int> badCharHeuristic(std::string pattern, int size){
	std::vector<int> bad_char(NUM_CHARS, -1);
	// Computes bad character array for BM algorithm
	for(int i = 0; i < size; i++){
		bad_char[(int)pattern[i]] = i;
	}
	return bad_char;
}

// ----- ALGORITHMS -----

std::vector<int> stringMatchingNaive(std::string text, std::string pattern){
	// Naive string matching algorithm with O(n*m)
	std::vector<int> found_indices;
	int n = text.length();
	int m = pattern.length();
	for(int i = 0; i < n-m+1; i++){
		for(int j = 0; j < m; j++){
			if(text[i+j] != pattern[j]){
				break;
			}
			if(j == m-1){
				found_indices.push_back(i);
			}
		}
	}
	return found_indices;
}

std::vector<int> stringMatchingKMP(std::string text, std::string pattern){
	// Knuth Morris Pratt string matching algorithm with O(n)
	std::vector<int> found_indices;
	int n = text.length();
	int m = pattern.length();

	// precompute prefix/suffic array
	std::vector<int> lps = computeLPSArray(pattern, m);
	
	int i = 0;
	int j = 0;
	while(i < n){
		if(pattern[j]==text[i]){
			i++;
			j++;
		}
		if(j==m){
			found_indices.push_back(i-j);
			j = lps[j-1];
		}
		else if(i<n && pattern[j]!=text[i]){
			if(j!=0){
				j = lps[j-1];
			} else{
				i ++;
			}
		}
	}
	return found_indices;
}

std::vector<int> stringMatchingBM(std::string text, std::string pattern){
	// Boyer Moore string matching algorithm with O(nm) in the worst
	// case, but O(n/m) in the best case
	std::vector<int> found_indices;
	int n = text.length();
	int m = pattern.length();

	std::vector<int> bad_char = badCharHeuristic(pattern, m);

	int s = 0;
	while(s<=n-m){
		int j = m - 1;
		while(j >= 0 && pattern[j] == text[s+j]){
			j--;
		}
		if(j<0){
			found_indices.push_back(s);
			if(s+m<n) s += (m-bad_char[(int)text[s+m]]);
			else s++;
		} else{
			int value = (j-bad_char[(int)text[s+j]]);
			if(value>1) s += value;
			else s++;
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

	std::string test_text = generateBinaryString(100, 0.5);
	std::string test_pat = generateBinaryString(3, 0.5);
	std::vector<int> sol = stringMatchingBM(test_text, test_pat);
	std::cout <<test_text<<std::endl;
	std::cout << test_pat<<std::endl;
	std::cout <<"Indices found at:"<<std::endl;
	for (auto i = sol.begin(); i != sol.end(); i++){
		std::cout << *i << ' ';
	}
	std::cout << std::endl;

	MPI_Finalize();
	return 0;
}

