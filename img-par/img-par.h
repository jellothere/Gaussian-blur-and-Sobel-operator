#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <dirent.h>
#include <fstream>
#include <string.h>
#include <chrono>
#include <omp.h>
#include "./BmpUtils/BmpVectorImage.hpp"
#include "./BmpUtils/BmpManager.hpp"

using namespace std;
using namespace std::chrono;
using clk = chrono::high_resolution_clock;

// Command parser
void parse(int &mode, DIR *&input_dir, DIR *&output_dir, int argc, char *argv[]);
// Gaussian blur filter
vector<u_int8_t> gaussian_blur_par(vector<u_int8_t> initial_image, int width, int heigth);
// Sobel operator
vector<u_int8_t> sobel_operator_par(vector<u_int8_t> initial_image, int width, int height);
