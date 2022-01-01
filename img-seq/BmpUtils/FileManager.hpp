#pragma once
#include <string>
#include "Exceptions/FileException.hpp"
#include <fstream>
#include <vector>


/**
 * CLASS FILEMANAGER
 * 
 * - Functionality summary:
 * FileManager is in chare of opening, reading and writing files.
 * Member functions of FileManager are not thread-safe, and therefore no more than one instance should be created
 * accessing the same file.
 * The FileManager is independent to the BMP module: it can read and write any file, and can be called from anywhere.
 * FileManager relies on the fstream library, which is a standard library in C++.
 * 
 * 
 * - Special optimization mechanisms implemented:
 * The basic unit of information is the byte, and so the files are read and written in bytes. This ensures the fastest
 * of read/write speeds (because no data conversions are in place) while also providing a data structure (vector of bytes)
 * which can be easily processed by other Bmp modules (such as the BmpHeaderDecoder).
 * 
 * @authors Group 89-02
 * @version 1.0
 * @since   28-11-2020
 */
class FileManager{
    private:

    public:
        /**
         * Reads a file as a byte sequence.
         * @param file_name full path to the file to read.
         * @return vector containing all bytes read from the file.
         * In case of error, returns an empty vector.
         * @see BmpHeaderDecoder::decode_BMP(), which catches this error.
         * TODO: We could throw an exception! Discuss it.
         */ 
        
        vector<u_int8_t> get_bytes_from_file(std::string file_name);
        /**
         * Writes a sequence of bytes to a file.
         * @param byte_vector bytes to write in the file.
         * @param full_file_name full path to the file where to write the bytes.
         * @return 0 if success, 1 if errors.
         */ 
        int save_bytes_to_file(vector<u_int8_t> byte_vector, std::string full_file_name);
};