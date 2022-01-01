#include "BmpHeaderDecoder.hpp"
#include <iostream>

using namespace std;


bool BmpHeaderDecoder::equals(u_int32_t a, u_int32_t b){
    return a==b;
}

bool BmpHeaderDecoder::greater_eq_than(u_int32_t a, u_int32_t b){
    return a>=b;
}

//TODO Hashed search
map<string, pair<pair<u_int8_t, u_int8_t>, pair<u_int32_t, BmpHeaderDecoder::funptr>>> BmpHeaderDecoder::offset_header_dictionary = {
    /*BMP header dictionary, format: 
    *       {key string, {value offset, value length}, {value constraint, function pointer constraint}} 
    * key -> primary key, human readable format
    * value offset -> byte number where first byte of the element is.
    * value length -> number of bytes the elements is represented in.
    * value constraint -> resource number on which the bytes the entry refers to are constrained.
    * function pointer constraint -> function pointer pointing to a function "equals" or "greater_eq_than" which refers to the operator of the constraint
    * 
    * Example: {FileSize, {{0, 2}, {1, greater_eq_than}}}
    *   ---> The file size of the BMP starts at byte 0, has a length of 2 bytes. 
    *        The value of the file (the full numerical value, not its bytes) size must be greater or equal to 1 byte.
    * 
    * Note that {0, greater_eq_than} means there is no constraint in place, because all unsigned bytes will always meet that constraint.
    */
    {"BMchars", {{0,2}, {0x4D42,equals}}},              //Characters 'B'and'M'. (exactly that value, stated by the problem)
    {"FileSize", {{2,4}, {1,greater_eq_than}}},         //Size of file. (any number as long as it is not 0, implicit)
    {"Reserved", {{6,4}, {0,greater_eq_than}}},         //Reserved. (may have any value, not constrained)
    {"ImgData", {{10,4}, {54,greater_eq_than}}},        //Start of image data (at least starts at byte 54, considering 0 the first, stated by the problem)
    {"BMHeaderSize", {{14,4}, {0, greater_eq_than}}},   //Size of the bitmap header. (any value, not constrained)
    {"Width", {{18,4}, {1,greater_eq_than}}},           //Width in pixels. (at least one pixel, implicit)
    {"Height", {{22,4}, {1,greater_eq_than}}},          //Height in pixels. (at least one pixel, implicit)
    {"NumPlanes", {{26,2}, {1,equals}}},                //Number of planes. (only one, stated by the problem)
    {"PointSize", {{28,2}, {24,equals}}},               //Point size in bits. (must be exactly 24, stated by the problem)
    {"Compression", {{30,4}, {0, equals}}},             //Compression. (exactly 0, stated by the problem)
    {"ImageSize", {{34,4}, {1, greater_eq_than}}},      //Image size. (we will accept any number greater than 0, implicit)
    {"HRes", {{38,4}, {0, greater_eq_than}}},           //Horizontal resolution (this is related to printable format, we will not constraint this.)
    {"VRes", {{42,4}, {0, greater_eq_than}}},           //Vertical resolution (same as with horizontal resolution, not relevant and thus not constrained.)
    {"ColorTableSize", {{46,4}, {0, greater_eq_than}}}, //Color table size (related to number of colors in palette. Not relevant and thus not constrained.)
    {"ImpColorCounter", {{50,4}, {0, greater_eq_than}}} //Important color counter. (0 means all colors are important, but might not be the case. Not constrained.)
};


//Not used, just for debug
u_int32_t BmpHeaderDecoder::get_number_planes(){
    return get_value_BMP_header("NumPlanes");
}

//Not used, just for debug
u_int32_t BmpHeaderDecoder::get_point_size(){
    return get_value_BMP_header("PointSize");
}

//Not used, just for debug
u_int32_t BmpHeaderDecoder::get_compression(){
    return get_value_BMP_header("Compression");
}

u_int32_t BmpHeaderDecoder::get_file_size(){
    return get_value_BMP_header("FileSize");
}

u_int32_t BmpHeaderDecoder::get_start_image_data(){
    return get_value_BMP_header("ImgData");
}

u_int32_t BmpHeaderDecoder::get_width(){
    return get_value_BMP_header("Width");
}

u_int32_t BmpHeaderDecoder::get_height(){
    return get_value_BMP_header("Height");
}


u_int32_t BmpHeaderDecoder::get_value_BMP_header(string keyword){
    //We query the dictionary for the offset and length of the header field we desire to get specified in keyword.
    pair<u_int8_t, u_int8_t> values = offset_header_dictionary.at(keyword).first;
    //We call get_bytes_at position to get the bytes stored in the vector of bytes we previosuly revieved to decode
    //and which are currently stored in file_bytes
    vector<u_int8_t> result = get_bytes_at_position(values.first,values.second);
    //The result is the combination of two (or four) separate bytes, we will join them in a 4-byte (32-bit) word,
    //considering they were represented in Little-Endian.
    u_int32_t to_return;
    if(values.second==2){ //If they are 2-byte long
        //Debug point
        //cout<<static_cast<unsigned>(result[1])<<" "<<static_cast<unsigned>(result[0])<<endl;
        
        to_return = result[1]<<8 | result[0];
    }else{ //If they are 4-byte long
        //Debug point
        //cout<<static_cast<unsigned>(result[3])<<" "<<static_cast<unsigned>(result[2])<<" "<<static_cast<unsigned>(result[1])<<" "<<static_cast<unsigned>(result[0])<<endl;
        
        to_return = result[3]<<24 | result[2]<<16 | result[1]<<8 | result[0];
    }
    //Debug point
    //cout<<"Returning "<<to_return<<endl;
    return to_return;
}

bool BmpHeaderDecoder::is_header_valid(){
    //We need to check the values are correct by looking at the constraints.
    //If any of them is wrong, the function pointed by the function pointer will return false, and thus
    //the file will be recognized as invalid and return false. The BmpParser will then neglect this file.
    
    //Default value: we assume the header is valid and then try to prove it is wrong.
    bool header_is_valid = true;

    //We traverse all entries in the dictionary
    for (auto& ii: offset_header_dictionary) {
        //We take the key of the dictionary entry
        string label = ii.first;
        //We take the value of the constraint and the function pointer defining the operator of the constraint.
        pair<u_int32_t, funptr> constraints = ii.second.second;
        
        //We take the value which the header section has in the BMP file.
        u_int32_t header_byte_value = get_value_BMP_header(label);

        /*And we check the constraint with the value obtained before.
        * Example: label = "ImageSize", constraints = {1, greater_eq_than}, header_byte_value = 128 56 1 0 (little endian) = 80000
        *       Then 80000 greater_eq_than 1 => 80000>=1 => True
        *       The value of the header is true.
        */
        bool meets_constraint = (constraints.second)(header_byte_value,constraints.first);
        
        //Debug point
        //cout<<":"<<meets_constraint<<endl;

        //If at any point a constraint does not meed, the result is false forever.
        header_is_valid = header_is_valid&&meets_constraint;
        
        //Debug point
        if(!meets_constraint){
            //cout<<"Header element with label " <<label <<" is invalid. Parsed "<< header_byte_value<<" expected boundary "<< constraints.first<<endl;
        }else{
            //cout<<label<<" has valid value "<<header_byte_value<<endl;
        }
        //cout<<endl;
    }
    //We return whether all header fields were correct.
    return header_is_valid;
}


vector<u_int8_t> BmpHeaderDecoder::get_bytes_at_position(u_int32_t position, u_int32_t length){
    vector<u_int8_t> to_return;
    for(u_int8_t ii=0; ii<length; ii++){
        to_return.push_back(this->file_bytes[position+ii]);
    }
    return to_return;
}

vector<u_int8_t> BmpHeaderDecoder::get_pixel_data(u_int32_t pixel_data_start){
    /** TODO: HACKY IMPLEMENTATION is possible: Code review needed.
     * Explanation for hacky idea:
     * We could split the this->file_bytes vector, but the best known c++ way of doing this has O(n) complexity, with n being the pixel bytes. Consider that the file might be enormous!!
     * In order to save some accesses we will just delete the bytes from the file_bytes vector which correspond to the header.
     * ONLY FOR OLD VERSION -> HOWEVER NOTE THAT ONCE THIS FUNCTION IS INVOKED THE BMPHEADERDECODER NO LONGER HAS VALID DATA AND IT MUST BE DESTRUCTED.
     */
    
    //Explanation for new safe version: the file_bytes vector is copied before deleting elements.
    //But this takes a time!
    vector<u_int8_t> copy_vector = this->file_bytes;

    //Debug point
    //cout<<"Pixel data start detected at "<<pixel_data_start<<endl;

    //We delete all bytes belonging to the header, so that we only keep those of the image bytes.
    for(u_int32_t ii=0; ii<pixel_data_start; ii++){
        copy_vector.erase(copy_vector.begin());
    }
    return copy_vector;
}


vector<u_int8_t> BmpHeaderDecoder::get_header_data(u_int32_t pixel_data_start){
/**
 * WARNING: UNTESTED, JUST FOR DEBUGGING PURPOSES
 * TODO: Remove for final version
*/
    //Here since the header is quite short, we will just use a c++ iterator and take a subvector of the whole file bytes.
    vector<u_int8_t> header_vector(this->file_bytes.begin(), this->file_bytes.begin()+pixel_data_start);
    return header_vector;
}



void BmpHeaderDecoder::set_decoder_vector_bytes(vector<u_int8_t> file_bytes){
    //Just a conventional setter method.
    /** TODO: add some error checking here!!*/ 

    this->file_bytes = file_bytes;
}

BmpHeaderDecoder::decoder_return BmpHeaderDecoder::decode_BMP(){
    if(file_bytes.size() == 0){
        //Return 1 if the vector of bytes was empty.
        cerr<<"Fatal error reading the file: Was it empty?"<<endl;
        decoder_return to_return;
        to_return.return_code = 1;
        return to_return;
    }
    if(!is_header_valid()){
        //Return 1 if the header was invalid.
        cerr<<"Error decoding the header: The file does not support this file format"<<endl;
        decoder_return to_return;
        to_return.return_code = 1;
        return to_return;
    }

    //From this point on, we know every field in the header has the desired correct values. 
    //We will now take the values that the BmpParser needs to build the image and return.

    decoder_return to_return;
    to_return.return_code = 0;
    to_return.width = get_width();
    to_return.height = get_height();
    int start_image_data = get_start_image_data();
    to_return.image_bytes = get_pixel_data(start_image_data);

    //Debug point
    //cout<<"---->Original file length:"<<this->file_bytes.size()<<endl;
    //cout<<"---->Header length:"<<get_header_data(start_image_data).size()<<endl;
    //cout<<"---->Image length:"<<to_return.image_bytes.size()<<endl;
    //cout<<"---->Sending width="<<to_return.width<<", height="<<to_return.height<<endl;
    //cout<<"Pixel data start is at byte "<<start_image_data<<endl;

    return to_return;

}

