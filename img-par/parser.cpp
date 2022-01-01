#include "img-par.h"

// Check that the number of parameters passed is exactly 3
void parseNumParam(int numParam){
    if(numParam != 4) // The name of the program must be also considered (1 + 3 param) 
    {
        cerr << "Wrong format:" << endl;
        cerr << "  image-seq operation in_path out_path" << endl;
        cerr <<"     operation: copy, gauss, sobel" << endl;
        exit(-1);
    }
}

// Check that the execution mode is either copy (0), gauss (1) or sobel (2)
void parseExecMode(int &mode, string m){
    // If exec mode is correct, we store it in mode variable passed by reference
    if(m == "copy")
        mode = 0;
    else if(m == "gauss")
        mode = 1;
    else if(m == "sobel")
        mode = 2;
    
    else
    {
        // At this point, error in exec mode found
        cerr << "Unexpected operation:" << m << endl;
        cerr << "  image-seq operation in_path out_path" << endl;
        cerr <<"     operation: copy, gauss, sobel" << endl;
        exit(-1);
    } 
}

// Check that the provided input and output directories are valid
void parseDir(DIR *&input_dir, DIR *&output_dir, string ind, string outd){
    
    // Converting the strings to constant character arrays (used when trying to
    // open the directories)
    const char *ind_c = ind.c_str();
    const char *outd_c = outd.c_str();

    // Trying to open the provided input directory
    // We save the input directory to be used
    input_dir = opendir(ind_c);
    if(input_dir == NULL)
    {
        cout << "Error 1" << endl;
        // At this point, input directory is not valid (does not exist or cannot be opened)
        cerr << "Input path: " << ind << endl;
        cerr << "Output path: " << outd << endl;
        cerr << "Cannot open directory " << ind << endl;
        cerr << "  image-seq operation in_path out_path" << endl;
        cerr <<"     operation: copy, gauss, sobel" << endl;
        exit(-1);
    }

    // Trying to open the provided output directory
    // We save the input directory to be used
    output_dir = opendir(outd_c);

    if(output_dir == NULL)
    {
        cout << "Error 2" << endl;
        // At this point, output directory is not valid (does not exist or cannot be opened)
        cerr << "Input path: " << ind << endl;
        cerr << "Output path: " << outd << endl;
        cerr << "Output directory " << outd << " does not exist" << endl;
        cerr << "  image-seq operation in_path out_path" << endl;
        cerr <<"     operation: copy, gauss, sobel" << endl;
        exit(-1);
    }
    
}

// Parse method: makes use of the previously defined functions to parse the provided image
void parse(int &mode, DIR *&input_dir, DIR *&output_dir, int argc, char *argv[]) {
    // Parsing the number of parameters
    parseNumParam(argc);
    // Parsing the execution mode
    parseExecMode(mode, argv[1]);
    // Parsing the input and output directories
    parseDir(input_dir, output_dir, argv[2], argv[3]);
}