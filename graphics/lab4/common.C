#include "common.h"


void print_vector(SbVec3f vec){
	std::cout <<" ("<<vec[0]<<","<<vec[1]<<","<<vec[2]<<")"<<std::endl;
}

float get_random_number(){
    float num;
    num = rand()/(float)(RAND_MAX+1);
    //num = -1 * num;
    return num;
}

void swap(float *a, float *b){
    float temp;
    temp = *a;
    *a = *b;
    *b = temp;

}

float calculate_determinant(float a, float b, float c){
	float d = 0;
        d = (b*b) - (4*a*c);
        if(d < 0) return -1;
        else return sqrt(d);
}

float calculate_solution(float d, float b, float a){
	float sol1, sol2;
        if(d == 0)
                return ((-b)/(2 * a));

        sol1 = (((-b)+d)/(2 * a));
        sol2 = (((-b)-d)/(2 * a));
        if(sol1 < 0 && sol2 < 0)
         return -1;
        if(sol1 < sol2)
         return sol1;
        else
         return sol2;

}

