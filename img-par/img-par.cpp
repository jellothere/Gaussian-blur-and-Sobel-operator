#include "img-par.h"
#include <omp.h>
#include <iomanip>

int main(int argc, char *argv[]){
  int mode; //copy = 0, gauss = 1, sobel = 2
     DIR *input_dir;
     DIR *output_dir;
     vector<u_int8_t> final_image;
     struct BmpVectorImage image;
     struct dirent *entry;

     // Parsing the introduced command
     parse(mode, input_dir, output_dir, argc, argv);

     string input_dir_string = argv[2];
     string output_dir_string = argv[3];

     //cout<<"Parsed successfully"<<endl;
     
     // Printing input and output directories information
     cout << "Input path: "  << argv[2] << endl;
     cout << "Output path: " << argv[3] << endl;
    
     //Decide precision of timers (decimal points)
     /** NOTE: This prevents exponential notation
      * It is just a workaround, talk in the group about this, it does not look right.
     */
     cout<<setprecision(10);

     while((entry = readdir(input_dir))!=NULL)
     {
         
          if(strcmp(entry->d_name, "..") == 0 ||strcmp(entry->d_name, ".") == 0){
               
               continue;
          }
          //cout<<"Starting new file"<<endl;

          // Checkpoints will be set in order to calculate the time of execution of each task
          double t1global = omp_get_wtime();
          
          // fullpath to the current file under analysis
          string filename = input_dir_string+"/"+entry->d_name;
          // Loading the image
          double t1 = omp_get_wtime();
          BmpManager manager;

          //cout<<"Starting to load the image "<<filename<<endl;

          image = manager.load_image(filename);

          //cout<<"Finished loading image"<<endl;

          double t2 = omp_get_wtime();  
          double load_time = (t2 - t1)*1000000;

          // Selecting among the modes of execution according to the received instruction
          if (mode == 0)
          {
               // copy operation does not need to get its execution time measured
               final_image = image.initial_image;
          }

          if (mode == 1)
          {
               //cout<<"Applying Gaussian blur"<<endl;
               t1 = omp_get_wtime();
               final_image = gaussian_blur_par(image.initial_image, image.width, image.height);
               t2 = omp_get_wtime();
               //cout<<"Finished Gaussian blur"<<endl;
          }
          double gauss_time = (t2 - t1)*1000000;

          if (mode == 2)
          {
               // The Gaussian blur filter is applied
               t1 = omp_get_wtime();
               final_image =  gaussian_blur_par(image.initial_image, image.width, image.height);
               t2 = omp_get_wtime();
               gauss_time = (t2 - t1)*1000000;

               //Afterwards, the Sobel operator is applied
               t1 = omp_get_wtime();
               final_image =  sobel_operator_par(final_image, image.width, image.height);
               t2 = omp_get_wtime();
          }
          double sobel_time = (t2 - t1)*1000000;
          
          // fullpath to the current file under analysis
          filename = output_dir_string+"/"+entry->d_name;
          // Saving the image
          t1 = omp_get_wtime();
          manager.save_image(filename, final_image);
          t2 = omp_get_wtime();
          double store_time = (t2 - t1)*1000000;

          double t2global = omp_get_wtime();
          double global_time = (t2global-t1global)*1000000;
          
          // Printing the timing information for the current file
          cout << "File: \"" << filename << "\" (time: " << global_time << ")" << endl;
          cout << "  Load time: " << load_time << endl;
          // Printing the Sobel operator time just if the execution mode specified at the command
          // line was "sobel", and printing gauss only if the execution was gauss or sobel.
          if(mode == 1){
               cout << "  Gauss time: " << gauss_time << endl;
          }
          if(mode == 2){
               cout << "  Gauss time: " << gauss_time << endl;
               cout << "  Sobel time: " << sobel_time << endl;
          }
          cout << "  Store time: " << store_time << endl<<endl;
     }

     cout<<"Finished all files"<<endl;

    // Closing the opened input and output directories
     closedir(input_dir);
     closedir(output_dir);
}