#include <iostream>
#include "FileManager.hpp"
#include "BmpManager.hpp"

using namespace std;


BmpVectorImage BmpManager::load_image(string file){
    //First, we load our parser with the file.
    this->bmp_parser.load_parse_file(file);
    //We obtain the pixel sequence from the loaded file.
    BmpVectorImage result = this->bmp_parser.get_BMP_pixel_sequence();
    if(result.initial_image.empty()){
        cerr<<"FATAL ERROR: The file was corrupted."<<endl; /** TODO: What to return here? We can do this with exceptions too.*/ 
        exit(-1);
    }
    return result;
}

/** TODO: Why was this function receiving a directory? It is set as a file now.*/
int BmpManager::save_image(string file, vector<u_int8_t>& final_image){
    //We load our parser with the file.
    this->bmp_parser.load_parse_file(file);
    //We save the parameter image in the file we loaded.
    int return_value = bmp_parser.save_BMP_pixel_sequence(final_image);
    return return_value;
}

/*int main(int argc,char* argv[]){
    //JUST A TEST!!
    (void)argc;
    BmpManager manager;
    vector<u_int8_t> result = manager.load_image(argv[1]).initial_image;
    
    if(result.empty()){
        cerr<<"Nothing could be loaded, the file was in an invalid format"<<endl;
        return -1;
    }
    /*for (vector<Pixel> it : result){
        for (Pixel jt: it){
            cout << static_cast<unsigned>(jt.b)<<" "<<static_cast<unsigned>(jt.g)<<" "<<static_cast<unsigned>(jt.r) << endl;
        }
    }
    //cout<<" "<<result.size()<<" "<<result[0].size()<<endl;

    //TEST--- Assume the filters and operators have been already applied and we get a Pixel vector again
    manager.save_image("input/output.bmp", result);

    
    return 0;
}*/