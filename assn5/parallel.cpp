#include <iostream>
#include <fstream>
#include <time.h>
#include <stdlib.h>
#include <mpi.h>
#include <unistd.h>
#include <algorithm>
#include <math.h>
#include <string>

const int MAX_ITERATION = 1000;
const int RESOLUTION = 512;
const bool INVERT_COLORS = false;
const std::string OUTPUT_FILE = "parallel_mandelbrot.ppm";

struct Color{
public:
	int r;
	int g;
	int b;
	
	Color(){r,g,b=0;}

	Color(int r, int g, int b){
		this->r = r;
		this->g = g;
		this->b = b;
	}
};

Color getColor(int iteration){
	int r = iteration%145;
	int g = iteration%200;
	int b = iteration%255;
	if(INVERT_COLORS){
		r = 255 - r;
		g = 255 - g;
		b = 255 - b;
	}
	return Color(r,g,b);
}

std::ofstream setupFile(){
	std::ofstream mandelbrot_file(OUTPUT_FILE);
	mandelbrot_file << "P3" << std::endl;
	mandelbrot_file << RESOLUTION << " " << RESOLUTION << std::endl;
	mandelbrot_file << "255" << std::endl;
	return mandelbrot_file;
}

void writeToFile(std::string message, std::ofstream &mandelbrot_file){
	mandelbrot_file << message;	
}

void plotImage(int** plot){
	std::ofstream mandelbrot_file = setupFile();
	for(int i = 0; i < RESOLUTION; i++){
		for(int j = 0; j < RESOLUTION; j++){
			Color c = getColor(plot[i][j]);
			std::string color = std::to_string(c.r) + " " + std::to_string(c.g) + " " + std::to_string(c.b) + " ";
			writeToFile(color, mandelbrot_file);
		}
		writeToFile("\n", mandelbrot_file);
	}
	mandelbrot_file.close();
}

int calculatePixel(int px, int py){
	double x0 = -2 + px * (2.5/RESOLUTION);
	double y0 = -1.25 + py * (2.5/RESOLUTION);
	double x = 0.0;
	double y = 0.0;
	int iteration = 0;
	while(x*x + y*y < 4 && iteration < MAX_ITERATION){
		double xtemp = x*x - y*y + x0;
		y = 2*x*y + y0;
		x = xtemp;
		iteration += 1;
	}
	return iteration;
}

int ** initPlot(int size){
	int ** plot = new int*[size];
	for(int i = 0; i < size; i++){
		plot[i] = new int[size];
	}
	return plot;
}

void mandelbrot(int world_size, int world_rank){
	int offset = RESOLUTION/world_size;
	int ** plot = initPlot(offset);
	for(int i = 0; i < offset; i++){
		for(int j = 0; j < offset; j++){
			plot[i][j] = calculatePixel(j, i*(world_size)+world_rank);
		}
	}
	int ** recv_data = initPlot(RESOLUTION);
	MPI_Gather(&plot,offset*offset,MPI_INT,&recv_data,offset*offset,MPI_INT,0,MPI_COMM_WORLD);
	if(world_rank==0) plotImage(plot);
}


int main(int argc, char** argv) {
	MPI_Init(&argc, &argv);
	srand(time(NULL));
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	int world_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

	mandelbrot(world_size, world_rank);
	MPI_Finalize();
	return 0;
}

