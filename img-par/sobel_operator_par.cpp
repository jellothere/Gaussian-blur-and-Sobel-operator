#include "img-par.h"

//defining mask matrix Mx
short mx [3][3] = { {1, 2, 1}, {0, 0, 0}, {-1, -2, -1}};
//defining mask matrix My
short my [3][3] = { {-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1} };
//weight value
int weight = 8;

//Method
vector<u_int8_t> sobel_operator_par(vector<u_int8_t> initial_image, int width, int height){

    //resulting vector
    vector<u_int8_t> result(initial_image.size());
 
    int padding = 0;
    if((width) % 4 != 0){
        padding = 4 - ((width) % 4);
    }

    //partial results variables
    int resXr=0;
    int resXg=0;
    int resXb=0;
    int resYr=0;
    int resYg=0;
    int resYb=0;

    //PRAGMA
    #pragma omp parallel for reduction(+: resXr, resXg, resXb, resYr, resYg, resYb) collapse(2) num_threads(16)
        //traversing the initial image
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j+=3) {

                //they are set to 0 each time we move to another position in the initial matrix
                    resXr= 0;
                    resXg= 0;
                    resXb= 0;
                    resYr= 0;
                    resYg= 0;
                    resYb= 0;

                //loops to calculate the sum of the partial results
                for(int s=-1; s<=1; s++){
                    for(int t=-1; t<=1; t++){
                        if((i+s)>=0 && (i+s)<height && (j+3*t)<width && (j+3*t)>=0){
                            //calculating partial results
                            resXr += mx[s+1][t+1] * initial_image[(i+s)*(width+padding)+ (j+3*t)];
                            resXg += mx[s+1][t+1] * initial_image[(i+s)*(width+padding)+(j+3*t)+1];
                            resXb += mx[s+1][t+1] * initial_image[(i+s)*(width+padding)+(j+3*t)+2];
                            resYr += my[s+1][t+1] * initial_image[(i+s)*(width+padding)+(j+3*t)];
                            resYg += my[s+1][t+1] * initial_image[(i+s)*(width+padding)+(j+3*t)+1];
                            resYb += my[s+1][t+1] * initial_image[(i+s)*(width+padding)+(j+3*t)+2];
                        }
                        
                    }
                }

                //the final value is the addition of the absolute values of the partial results
                result[(i*(width+padding))+j] = (abs(resXr) + abs(resYr))/weight;
                result[(i*(width+padding))+j+1] = (abs(resXg) + abs(resYg))/weight;
                result[(i*(width+padding))+j+2] = (abs(resXb) + abs(resYb))/weight;
            }
            //int nt= omp_get_num_threads();
            //std::cout<<"\n\nthreads =" <<nt << "\n";
            //std::cout<<"THREAD ID:"<<omp_get_thread_num();
        }

    return result;
}