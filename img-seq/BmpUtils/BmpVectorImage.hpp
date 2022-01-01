#pragma once
#include <vector>
#include <string>


//Struct used to communicate the final result to the main program.
struct BmpVectorImage{
    std::vector<u_int8_t> initial_image;
    int width;
    int height;
};