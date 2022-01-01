#include "img-seq.h"
//Weight
int w = 273;
//Mask matrix
short m[5][5] = {{1,4,7,4,1},{4,16,26,16,4},{7,26,41,26,7},{4,16,26,16,4},{1,4,7,4,1}};

vector<u_int8_t> gaussian_blur_seq(vector<u_int8_t> initial_image, int width, int height){
    //Variables representing each of the colors
    int red = 0;
    int green = 0;
    int blue = 0;
    //Final image
    vector<u_int8_t> final_image (initial_image.size());
    int padding = 0;
    //Calculating the padding
    if((width) % 4 != 0){
        padding = 4 - ((width) % 4);
    }
    //Moving through the image's vector
    for (int i=0 ; i < height; i++) {
        for (int j=0 ; j < width; j+=3) {
            //Double sum
            for (int s=-2 ; s < 3 ; s++) {
                for (int t = -2 ; t < 3 ; t++) {
                    //Checking we are not out-of-bounds
                    if((i+s)>=0 && (i+s)<height && (j+3*t)<width && (j+3*t)>=0){
                        //Applying the blurring operation
                        blue += m[s+2][t+2] * initial_image[((i+s)*(width+padding))+(j+3*t)];
                        green += m[s+2][t+2] * initial_image[((i+s)*(width+padding))+(j+3*t)+1];
                        red += m[s+2][t+2] * initial_image[((i+s)*(width+padding))+(j+3*t)+2];
                    }
                }
            }
            //Dividing by the weight and assigning to the final image variables
            final_image[(i*(width+padding))+j] = blue/w;
            final_image[(i*(width+padding))+j+1] = green /w;
            final_image[(i*(width+padding))+j+2] = red/w;
            //Setting the color variables to 0
            blue = 0;
            green = 0;
            red = 0;
        }
    }
    return final_image;
}
