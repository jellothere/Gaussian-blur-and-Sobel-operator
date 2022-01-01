#include <vector>
#include <iostream>
#include "FileManager.hpp"
#include "BmpParser.hpp"
#include <chrono>

using namespace std;

void BmpParser::load_parse_file(string file){
    this->related_file = file;
}

BmpVectorImage BmpParser::get_BMP_pixel_sequence(){
    auto t1 = chrono::high_resolution_clock::now(); /** TODO:FOR TIMING, REMOVE THIS LATER*/

    FileManager fileManager;
    BmpHeaderDecoder decoder;

    
    /** NOTE: Remove this, note for us: 
     * We cannot just recieve a pixel already, this would render the header processing
     * quite difficult and in fact would require a hardcoded process to lookup the header sections. 
     * Ultimately, the whole thing would be a mess.
     */
    //We request FileManager to open and read the file, and return a vector with all its bytes.
    vector<u_int8_t> result = fileManager.get_bytes_from_file(this->related_file);
   
    //Debug point
    /*for(unsigned char x :  result)  {
        cout << x << " : " << static_cast<unsigned>(x) << endl;
    };*/

    decoder.set_decoder_vector_bytes(result);
    
    //Debug point
    //You should set the member functions public before uncommenting this
    //cout <<"Number planes: "<< decoder.get_number_planes()<<endl;
    //cout <<"Point size: "<< decoder.get_point_size()<<endl;
    //cout <<"Compression: "<< decoder.get_compression()<<endl;
    
    //We decode the header we obtained before using BmpHeaderDecoder. We will get a struct decoder_return
    //which contains all the data needed to proceed (a return code, some relevant header fields...).
    struct BmpHeaderDecoder::decoder_return image_bytes = decoder.decode_BMP();

    //We check the decoder did not recognize the header as invalid.
    if(image_bytes.return_code!=0){
        //Header was invalid, nothing could be loaded then.
        return {};
    }

    //Saving the old header fields to use it later when saving the new file
    this->original_decoded_height_in_pixels = image_bytes.height;
    this->original_decoded_width_in_pixels = image_bytes.width;


    /** TODO: REMOVE THIS COMMENT, this is just an informative comment for us.*/
    //For one of the images the professors gave us, balloon.bmp did not have Start of Image Data = 54. 
    //In theory this is still a valid BMP file because the rest of fields are adjusted accordingly to that change, but maybe the professor 
    //wants us to detect this type of things. I am leaving this here just in case you think differently.
    //Note that if this happens, because the saving mechanism is restricted to follow Jose Daniel's criteria he specified by email, then
    //some bytes (around 25 in balloon.bmp) will be lost in the image (and it is visible if you open it with an image editor).
    if(this->original_decoded_width_in_pixels*this->original_decoded_height_in_pixels*3!=image_bytes.image_bytes.size()){
        //cerr<< "Rare event happened for the file "<<this->related_file<<", it is a valid BMP but will be partially corrupted on save because of the saving restrictions"<<endl;
    }
   
    //Debug point
    //cout<<"Preparing image with pixel Height="<<image_bytes.height<<", Width=" <<image_bytes.width<<endl;
    //cout<<"Size in bytes of image:"<<image_bytes.image_bytes.size()<<endl;

    //FOR TIMING, REMOVE THIS
    auto t2 = chrono::high_resolution_clock::now();
    auto load_time = chrono::duration_cast<chrono::microseconds>(t2 - t1);
    //cout << "*TIME:* Load time without final step: " << load_time.count() << endl;

    //We return the matrix of pixels (the vector of vectors of pixels) along with the width and height of the image.
    /** TODO: I felt bad writing this. Could both structs be combined?
     * Also change the above comment, it still talks about pixel matrices.
    */
    struct BmpVectorImage image_result;
    image_result.initial_image = image_bytes.image_bytes;
    image_result.height = image_bytes.height; //same height in bytes than in pixels
    image_result.width = image_bytes.width*3;
    return image_result;
}

int BmpParser::save_BMP_pixel_sequence(vector<u_int8_t> byte_vector){
    /** TODO: It does not detect any errors. Implement it!*/

    //Now that we have the image as a byte sequence, we will add the original header to it.
    //cout<<"Now adding the original BMP header to the image bytes"<<endl;
    //This function will append the header to our vector of bytes, so from this point on we
    //already have the whole BMP file as a byte vector!
    add_BMP_header(byte_vector);
    
    //Debug point
    /*for(u_int8_t x :  byte_vector)  {
        cout << x << " : " << static_cast<unsigned>(x) << endl;
    };*/
    //cout<<"With the header, the byte vector is now "<<byte_vector.size()<<" bytes long"<<endl;

    //Finally, we save the vector of bytes to the file which the parser is related to.
    /** TODO: What about errors here? I forgot to check if they occur*/

    FileManager file_manager;
    file_manager.save_bytes_to_file(byte_vector, this->related_file);

    return 0;
}

void BmpParser::add_BMP_header(vector<u_int8_t>& image_bytes){
    /** TODO: Could this function be improved? Code review needed.*/

    //Here, we will follow the professor guidelines and insert a header which follows a predefined criteria.
    //First, we obtain the header data stored from the original image.
    //Note that all data we have (width, height, file and image size...) are stored in 32-bit integers,
    //so we will have to manually divide those 32 bits in groups of 8... There is not a better way of doing this in C++ as far as I know.
    vector<u_int8_t> height_bytes;
    for(int ii=0; ii<4; ii++){
        height_bytes.push_back(((u_int32_t)this->original_decoded_height_in_pixels >>  ii*8) & (u_int32_t)0x000000FF);
    }
    
    //Debug point
    //cout<<"Initial height was: "<<this->original_decoded_height_in_pixels<<endl;
    //cout<<"Now separated into: "<<static_cast<unsigned>(height_bytes[0])<<" "<<(unsigned)height_bytes[1]<<" "<<(unsigned)height_bytes[2]<<" "<<(unsigned)height_bytes[3]<<endl<<endl;
    
    //Width conversion to 8-bit groups
    vector<u_int8_t> width_bytes;
    for(int ii=0; ii<4; ii++){
        width_bytes.push_back(((u_int32_t)this->original_decoded_width_in_pixels >>  ii*8) & 0xFF);
    }

    //Debug point
    //cout<<"Initial width was: "<<this->original_decoded_width_in_pixels<<endl;
    //cout<<"Now separated into: "<<static_cast<unsigned>(width_bytes[0])<<" "<<(unsigned)width_bytes[1]<<" "<<(unsigned)width_bytes[2]<<" "<<(unsigned)width_bytes[3]<<endl<<endl;

    //Image size conversion to 8-bit groups.
    u_int32_t image_size = image_bytes.size();
    vector<u_int8_t> image_size_bytes;
    for(int ii=0; ii<4; ii++){
        image_size_bytes.push_back(((u_int32_t)image_size >>  ii*8) & 0xFF);
    }

    //Now we calculate, knowing the header must be 54 bytes, the file size. Also turn it into 8-bit groups.
    u_int32_t file_size = image_size + 54;
    vector<u_int8_t> file_size_bytes;
    for(int ii=0; ii<4; ii++){
        file_size_bytes.push_back(((u_int32_t)file_size >>  ii*8) & 0xFF);
    }

    //The vector is requested to follow this format.
    //Remember: LITTLE ENDIAN is used.
    vector<u_int8_t> header_vector = {66,77, //B,M chars
        file_size_bytes[0],file_size_bytes[1],file_size_bytes[2],file_size_bytes[3], //Total file size
        0,0,0,0, //Value 0
        54,0,0,0, //Value 54
        40,0,0,0, //Value 40
        width_bytes[0],width_bytes[1],width_bytes[2],width_bytes[3], //Width in pixels
        height_bytes[0],height_bytes[1],height_bytes[2],height_bytes[3], //Height in pixels
        1,0, //Value 1
        24,0, //Value 24
        0,0,0,0, //Value 0
        image_size_bytes[0],image_size_bytes[1],image_size_bytes[2],image_size_bytes[3], //Image size in bytes
        19,11,0,0, //Value 2835
        19,11,0,0, //Value 2835
        0,0,0,0,//Value 0
        0,0,0,0 //Value 0
    };
    
    //Note that we need to start inserting the header in reverse order to maintain the correct byte order in the file.
    for (vector<u_int8_t>::reverse_iterator i = header_vector.rbegin(); i != header_vector.rend(); ++i ) { 
        image_bytes.insert(image_bytes.begin(), *i);
        //Debug point
        //cout<<*i;
    }
    
    //Debug point
    //cout<<"Finished adding the header"<<endl;
}