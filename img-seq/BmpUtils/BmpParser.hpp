#pragma once
#include <string>
#include <vector>
#include <iostream>
#include "BmpHeaderDecoder.hpp"
#include "BmpVectorImage.hpp"

/**
 * CLASS BMPPARSER
 * 
 * - Functionality summary:
 * BmpParser is in charge of loading and saving BMP files by coordinating the functionalities
 * of BmpHeaderDecoder and FileManager.
 * It has a local view over a single BMP file, in contrast to the global view of BmpManager.
 * It is in charge of managing the conversion between the byte-level data structures and the 
 * pixel-level data structures.
 * Every BMP file returned as a matrix of pixels, if any, is always fully correct according to the
 * statement constraints.
 * 
 * @authors Group 89-02
 * @version 2.0
 * @since   2-12-2020
 */ 
class BmpParser{
    private:
        /**
         * Each BmpParser is related to a single file which can externally be changed by a setter.
         */
        std::string related_file;
        /**
         * TODO: this should not be necessary, we can take it from the pixel matrix! It works right now tho.
         * 
         * Data retrieved from the decoder, which will later be needed to save the file with its header again.
         * */
        u_int32_t original_decoded_width_in_pixels;
        u_int32_t original_decoded_height_in_pixels;
    
    public:
        /**
         * //TODO: what we return for an error is not quite convincing me...
         * 
         * Returns, if no errors, a completely correct byte vector using the internal loaded related_file.
         * @return If success, correct byte vector with height and width determined by the BMP file.
         * If errors, an empty vector.
         */ 
        struct BmpVectorImage get_BMP_pixel_sequence();

        /**
         * TODO: Caution I forgot to define what is an error here, always returns 0.
         * 
         * Saves an image specified as a pixel matrix to the internal loaded related_file.
         * @param pixel_vector image to save in file
         * @return 0 if success, 1 if errors.
         */ 
        int save_BMP_pixel_sequence(std::vector<u_int8_t> pixel_vector);

        /**
         * Setter method for related_file
         * @param file
         * @return void
         */
        void load_parse_file(std::string file);

        /**
         * Adds a valid BMP header to a vector of bytes corresponding to an image pixels.
         * TODO: It follows the criteria specified by email, but it could also have been implemented so that
         *      it reuses the same header as before, which would have been better because reasons specified somewhere else.
         * 
         * @param image_bytes vector of bytes of an image pixels (header not included)
         * @return void
         */
        void add_BMP_header(std::vector<u_int8_t>& image_bytes);
};