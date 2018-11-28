#include <iostream>
#include <fstream>
#include <sstream>
#include <time.h>
#include <stdlib.h>
#include <mpi.h>
#include <unistd.h>
#include <algorithm>
#include <math.h>
#include <string>
#include <vector>
#include <chrono>
#define MCW MPI_COMM_WORLD
#define ANY MPI_ANY_SOURCE


using Clock = std::chrono::steady_clock;
using std::chrono::time_point;
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using namespace std::literals::chrono_literals;

// ----- CONSTANTS -----

std::string FILE_SHAKESPEARE = "sample_strings/shakespeare.txt"; // Complete works of shakespeare
std::string FILE_DNA1 = "sample_strings/dna1.fasta"; // Dna sequence
int BINARY_STRING_LENGTH = 10000000; // Length of binary strings for study
int SHAKESPEARE_STRING_LENGTH = 100000; // Length of shakespeare string
int DNA_STRING_LENGTH = 100000; // Length of DNA string
int MIN_M = 1; // Smallest length of pattern for study
int MAX_M = 100000; // Largest length of pattern for study
int NUM_CHARS = 256; // number of characters in the alphabet used
bool STUDY_BINARY = true; // whether or not to run binary study
bool STUDY_SHAKESPEARE = true;
bool STUDY_DNA = true;
int VERBOSITY = 3;
int NAIVE = 42;
int KMP = 44;
int BM = 61;


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

std::string readFileToString(std::string filename){
	std::ifstream ifs (filename, std::ifstream::in);
	std::stringstream sstr;
	sstr << ifs.rdbuf();
	return sstr.str();
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

// ----- STUDY -----

void runAlgorithm(std::string text, int m, int world_rank, int world_size, int alg_code){
        std::vector<double> study_data;
        study_data.push_back((double)alg_code);
        study_data.push_back((double)m);
        time_point<Clock> start = Clock::now();
        int seg_start = (text.length()/world_size)*world_rank;
        int seg_length = (text.length()/world_size);
        if(alg_code == NAIVE){
                stringMatchingNaive(text.substr(seg_start, seg_length), text.substr(text.length()-m));
        } else if(alg_code == KMP){
                stringMatchingKMP(text.substr(seg_start, seg_length), text.substr(text.length()-m));
        } else if(alg_code == BM){
                stringMatchingBM(text.substr(seg_start, seg_length), text.substr(text.length()-m));
        }
        time_point<Clock> end = Clock::now();
        milliseconds time = duration_cast<milliseconds>(end-start);
        study_data.push_back((double)time.count());
        MPI_Send(&study_data[0], 3, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
}

void tallyResults(int world_rank, int world_size){
        // If root, gather up timing data, sum and output it
        // data is sent as an array of floats, [ALGORITHM_CODE, m, time]
        // if not root, wait until root gives the go signal
        // TODO

}

void runBinaryRandomStudy(int world_rank, int world_size){
	std::cout<<"-----Binary Random Study-----"<<std::endl;
	std::string binary_random;
	if(world_rank == 0){
		if(VERBOSITY>1) std::cout<<"Generating random binary string..."<<std::endl;
		binary_random = generateBinaryString(BINARY_STRING_LENGTH, 0.5);
		for(int i = 1; i < world_size; i++){
			MPI_Send(binary_random.c_str(), binary_random.size(), MPI_CHAR, i, 0, MPI_COMM_WORLD);
		}
	} else{
		int size = BINARY_STRING_LENGTH;
		MPI_Recv(&binary_random, size, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}
	for(int i = MIN_M; i < MAX_M; i *= 2){
		runAlgorithm(binary_random, i, world_rank, world_size, NAIVE);
		runAlgorithm(binary_random, i, world_rank, world_size, KMP);
		runAlgorithm(binary_random, i, world_rank, world_size, BM);
	}
	tallyResults(world_rank, world_size);
}

void runBinaryRegularStudy(int world_rank, int world_size){
	std::cout<<"-----Binary Regular Study-----"<<std::endl;
	std::string binary_regular;
	if(world_rank == 0){
		if(VERBOSITY>1) std::cout<<"Generating regular binary string..."<<std::endl;
		binary_regular = generateBinaryString(BINARY_STRING_LENGTH, 0.9999);
		for(int i = 1; i < world_size; i++){
			MPI_Send(binary_regular.c_str(), binary_regular.size(), MPI_CHAR, i, 0, MPI_COMM_WORLD);
		}
	} else{
		int size = BINARY_STRING_LENGTH;
		MPI_Recv(&binary_regular, size, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}
	for(int i = MIN_M; i < MAX_M; i *= 2){
		runAlgorithm(binary_regular, i, world_rank, world_size, NAIVE);
		runAlgorithm(binary_regular, i, world_rank, world_size, KMP);
		runAlgorithm(binary_regular, i, world_rank, world_size, BM);
	}
	tallyResults(world_rank, world_size);
}

void runShakespeareStudy(int world_rank, int world_size){
	std::cout<<"-----Shakespeare Study-----"<<std::endl;
	std::string shakespeare;
	if(world_rank == 0){
		if(VERBOSITY>1) std::cout<<"Reading Shakespeare string from file..."<<std::endl;
		shakespeare = readFileToString(FILE_SHAKESPEARE);
		for(int i = 1; i < world_size; i++){
			MPI_Send(shakespeare.c_str(), shakespeare.size(), MPI_CHAR, i, 0, MPI_COMM_WORLD);
		}
	} else{
		int size = SHAKESPEARE_STRING_LENGTH;
		MPI_Recv(&shakespeare, size, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}
	for(int i = MIN_M; i < MAX_M; i *= 2){
		runAlgorithm(shakespeare, i, world_rank, world_size, NAIVE);
		runAlgorithm(shakespeare, i, world_rank, world_size, KMP);
		runAlgorithm(shakespeare, i, world_rank, world_size, BM);
	}
	tallyResults(world_rank, world_size);
}

void runDNAStudy(int world_rank, int world_size){
	std::cout<<"-----DNA Study-----"<<std::endl;
	std::string dna;
	if(world_rank == 0){
		if(VERBOSITY>1) std::cout<<"Reading dna string from file..."<<std::endl;
		dna = readFileToString(FILE_DNA1);
		for(int i = 1; i < world_size; i++){
			MPI_Send(dna.c_str(), dna.size(), MPI_CHAR, i, 0, MPI_COMM_WORLD);
		}
	} else{
		int size = DNA_STRING_LENGTH;
		MPI_Recv(&dna, size, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}
	for(int i = MIN_M; i < MAX_M; i *= 2){
		runAlgorithm(dna, i, world_rank, world_size, NAIVE);
		runAlgorithm(dna, i, world_rank, world_size, KMP);
		runAlgorithm(dna, i, world_rank, world_size, BM);
	}
	tallyResults(world_rank, world_size);
}

void runStudy(int world_rank, int world_size){
        runBinaryRandomStudy(world_rank, world_size);
        runBinaryRegularStudy(world_rank, world_size);
        runShakespeareStudy(world_rank, world_size);
        runDNAStudy(world_rank, world_size);
}

// ----- MAIN -----

int main(int argc, char** argv) {
	MPI_Init(&argc, &argv);
	srand(time(NULL));
	int world_size;
	MPI_Comm_size(MCW, &world_size);
	int world_rank;
	MPI_Comm_rank(MCW, &world_rank);
	
	runStudy(world_rank, world_size);
	
	MPI_Finalize();
	return 0;
}

