#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include "FileManager.hpp"
#include "Exceptions/FileException.hpp"

using namespace std;

vector<u_int8_t> FileManager::get_bytes_from_file(string file_name){
    //This will hold 1 byte, used to create the vector of bytes.
    u_int8_t one_byte_buffer;

    //Vector of bytes to return.
    vector <u_int8_t> file_bytes; 

    //File stream is opened with the following flags:
    //  ios::in -> File opened for reading
    //  ios::binary -> Operations performed in binary mode instead of text mode
    ifstream ifst (file_name, ios::in | ios::binary);
    
    //We check the file was successfully opened. 
    if (ifst.is_open()){
        //We read byte by byte.
        while(ifst.read((char*)&one_byte_buffer, sizeof(u_int8_t))){
            file_bytes.push_back(one_byte_buffer);
            
            //Debug point
            //cout<<one_byte_buffer<<" ";
        }
        return file_bytes;

    }else{ //If the file was not opened.
        return {};
    }
}

int FileManager::save_bytes_to_file(vector<u_int8_t> byte_vector, std::string full_file_name){   
    //File stream is opened with the following flags:
    //  ios::out -> File opened for writing
    //  ios::binary -> Operations performed in binary mode instead of text mode
    //Note that if no file existed with that name, it is created.
    std::ofstream ofst (full_file_name, ios::out|ios::binary);

    //Debug point
    //cout<<"Starting to write the bytes to the file "<<full_file_name<<" for "<<byte_vector.size()<<" iterations "<<endl;
    //cout<<"First byte is "<<byte_vector[0]<<endl; //If this prints "B", high chances of the file being correct
    
    //We write each byte to the file.
    for (size_t n=0; n < byte_vector.size(); n++){
        ofst.write((char*)(&(byte_vector[n])), sizeof(u_int8_t));
        
        //Debug point
        //cout<<"Writing "<<byte_vector[n]<<" : "<<static_cast<unsigned>(byte_vector[n])<<endl;
    }
    //Debug point
    //cout<<"Finished writing in file"<<endl;

    //We return whether the file was correctly opened or not.
    return ofst.good();
}

