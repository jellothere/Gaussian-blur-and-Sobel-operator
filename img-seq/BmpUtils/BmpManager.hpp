#pragma once
#include <vector>
#include <string>
#include "BmpParser.hpp"
#include "BmpVectorImage.hpp"

/**
 * CLASS BMPMANAGER
 * 
 * - Functionality summary:
 * BmpManager plays the role of an intermediary and coordinator between the filter application and a set of BmpParsers.
 * It is the main class of the BmpUtils, and has a global view over all the BMP file requests that are currently ongoing.
 * It can be used to obtain a matrix of Pixels corresponding to a valid BMP file.
 * 
 * TODO: Implement Singleton
 * 
 * @authors Group 89-02
 * @version 2.0
 * @since   2-12-2020
 * @see BmpParser
 */ 

class BmpManager{
    private:   
        /**
         * List of BmpParsers in memory. In the sequential version, there will only exist one parser
         * because only one file can be processed at a time.
         * TODO: Team meeting discussion about the parallel version idea.
         * 
         * In the sequential version:
         * ---> After requesting to load an image, the referenced parser is loaded with a new file.
         * ---> After requesting to save a pixel array to a file, the reference to the parser is just maintained in memory.
         *         //NOTE: This would change in the parallel version idea, where the parser is expelled from the queue.
         */ 
        BmpParser bmp_parser;
    public:
        /**
         * This method returns a pixel matrix corresponding to a valid BMP file specified as a full-path to that file.
         * @param file valid full path to the file.
         * @return Matrix of pixel structs, with width and height taken from the parameter file.
         */
        BmpVectorImage load_image(std::string file);
        /**
         * This method saves a pixel matrix to the disk as a BMP file in the path given as a parameter.
         * @param file valid full path to the file.
         * @param final_image Image as a pixel matrix reference.
         * @return 0 if success. -1 if errors.
         */
        int save_image(std::string file, std::vector<u_int8_t>& final_image);
};