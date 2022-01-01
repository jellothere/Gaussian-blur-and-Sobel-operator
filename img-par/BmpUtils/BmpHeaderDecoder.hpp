#pragma once
#include <string>
#include <vector>
#include <map>
#include <functional>

/**
 * CLASS BMPHEADERDECODER
 * 
 * - Functionality summary:
 * BmpHeaderDecoder is a class which processes and validates headers of BMP files when given the BMP file as a byte vector.
 * It therefore works in the byte-level, and every instance of it works with only one file.  It has no notion about pixels
 * (the final data structure we provide our result in the BmpParser).
 * 
 * - Special optimization mechanisms implemented:
 * The dictionary offset_header_dictionary which holds the whole data about headers consists in a map data structure. In essence, this is
 * a Hash Table where the primary key is the hash of its first entry, a globally unique string which identifies each header field.
 * Therefore the dictionary has very low search time cost and the whole class has been implemented around it: every data we may 
 * need about the header fields is contained in there and with just one select query we can get all data we need (constraints, values, etc).
 * It also makes use of function pointers which simpify and speed up the whole process substantially.
 * 
 * 
 * @authors Group 89-02
 * @version 2.0
 * @since   2-12-2020
 */ 
class BmpHeaderDecoder{
    private:
        //Auxiliary functions which are referenced by function pointers in the dictionary.
        //Returns if two integers are equal.
        static bool equals(u_int32_t a, u_int32_t b);
        //Returns if the first integer is greater or equal than the second.
        static bool greater_eq_than(u_int32_t a, u_int32_t b);

        //Function pointer type definition, it takes two integers and processes them according to one of the functions "equals" or "greater_eq_than"
        typedef bool (*funptr)(u_int32_t, u_int32_t);

        /**
         * The dictionary containing all needed data about headers.
         * @see offset_header_dictionary implementation in BmpHeaderDecoder.cpp for a detail explanation on its functionalities.
         */ 
        static std::map<std::string, std::pair<std::pair<u_int8_t, u_int8_t>, std::pair<u_int32_t, funptr>>> offset_header_dictionary;
        
        /**
         * Vector containing the whole vector of bytes which the decoder has to process.
         * Many functions rely on it, so it is kept as a private attribute for simplicity and clarity.
         */ 
        std::vector<u_int8_t> file_bytes;

        
        /**
         * Auxiliary functions, they do not rely on a hardcoded byte number, but rather invoke the function
         * get_value_BMP_header() with the key they want to get from the dictionary.
         * @see get_value_BMP_header()
         */ 
        u_int32_t get_start_image_data();
        u_int32_t get_number_planes();
        u_int32_t get_point_size();
        u_int32_t get_file_size();
        u_int32_t get_compression();
        u_int32_t get_height();
        u_int32_t get_width();


        /**
         * Function used to obtain data from the dictionary and extract the appropiate value from the byte vector.
         * It is called from the auxiliary functions above.
         * For example:
         * For the key "Width", it looks up in the dictionary in which bytes and how long is that field (18 offset, 4 bytes-long)
         * and later extracts from file_bytes those bytes (ex:0 0 0 133 for a given image) and returns the complete numerical value
         * of those bytes by casting the 4-byte group into a 32-bit numerical value: 0 0 0 133 --> 133 pixels width.
         * Note that the values in the header are in Little Endian (133 0 0 0) so before converting those values the function reorders them
         * as shown in the above example to easy the conversion.
         * 
         * @param keyword primary key from the dictionary.
         * @return value of the key in file_bytes once converted to 32-bit.
         */ 
        u_int32_t get_value_BMP_header(std::string keyword);
        
        /**
         * Auxiliary function used by others to extract bytes from file_bytes given an offset and a lenght.
         * The extracted bytes belong to following interval: [position, position+length) <-- Last one is open, not included
         * 
         * @param position offset of the byte where to start
         * @param length number of bytes to extract starting from position
         * @return vector of bytes with those bytes belonging to the interval [position, position+length) in file_bytes. 
         */ 
        std::vector<u_int8_t> get_bytes_at_position(u_int32_t position, u_int32_t length);

        /**
         * Function which traverses the dictionary and, extracting the data related to the constraints
         * (the value of the constraint and the function pointer to compare the value with) checks for
         * each of the entries the validity of their values and returns true if and only of all the header
         * fields (in file_bytes) are correct.
         * 
         * REPORT: This function is very optimized and checks the validity of all header fields, more than what we are asked for!!
         * 
         * @return true if all the header fields are valid, false if at least one is not correct according to the constraints.
         */ 
        bool is_header_valid();

        /**
         * This function is used to extract from file_bytes only the bytes which belong to image pixels,
         * discarding the header.
         * @param pixel_data_start position where the pixels start
         * @return vector of bytes consisting in all bytes where pixels are stored.
         */
        std::vector<u_int8_t> get_pixel_data(u_int32_t pixel_data_start);

        /**
         * TODO: Remove this, not needed
         * This function is used just for debugging purposes.
         * It extracts only the header bytes from file_bytes, similarly to get_pixel_data()
         * @param pixel_data_start position where the pixels start
         * @return vector of bytes consisting in all bytes belonging to the header.
         */ 
        std::vector<u_int8_t> get_header_data(u_int32_t pixel_data_start);

        
    public:
        /**
         * Setter method for the private field file_bytes.
         * @param file_bytes vector of bytes to store the decoder shall work with.
         */ 
        void set_decoder_vector_bytes(std::vector<u_int8_t> file_bytes);
        
        /**
         * This struct defines a composition of data which the BmpParser wants to know once the 
         * BmpHeaderDecoder finishes its execution. It is returned to it once the member function
         * decode_BMP() is called.
         * @see decode_BMP()
         */ 
        struct decoder_return{
            /*Return code of the decoder.
                0 if SUCCESS. 
                1 if errors in header because not meeeting the constraints (compression, point size...), 
                    implies corruption of the file or a non-supported format.
            */
            int return_code;
            //Bytes which contain only pixels
            std::vector<u_int8_t> image_bytes;
            //Width and height in pixels (where each pixel is 3 bytes, one for each color).
            int width, height;
        };

        /**
         * Main function of BmpHeaderDecoder. Once called:
         * - Checks the validity of the header @see is_header_valid
         * - If valid, extracts the data needed from the byte vector file_bytes.
         * - Encapsulates image pixels as a byte vector and other needed data in BmpParser
         *   in a struct decoder_return.
         * 
         * @return decoder_return struct, with a return code (0 if success, 1 if errors), the image byes
         *  as a vector, and the width and height of the image in pixels.
         */ 
        struct decoder_return decode_BMP();

};