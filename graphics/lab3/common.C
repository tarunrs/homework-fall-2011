#include "common.h"


void print_vector(SbVec3f vec){
	std::cout <<" ("<<vec[0]<<","<<vec[1]<<","<<vec[2]<<")"<<std::endl;
}

float get_random_number(){
    float num;
    num = rand()/(float)(RAND_MAX+1);
    return num;
}

void swap(float *a, float *b){
    float temp;
    temp = *a;
    *a = *b;
    *b = temp;

}
