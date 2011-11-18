#include "light.h"

Light::Light(){
	position.setValue(0.0,0.0,0.0);
}

Light::Light(SbVec3f pos, float intens, SbVec3f clr, float rad){
	position = pos;
	intensity = intens;
	color = clr;
	radius = rad;
}
void Light::print_details(){
	std::cout<<"Light Position : "<<position[0]<<
	" , "<<position[1]<<
	" , "<<position[2]<< std::endl;
}

bool Light::intersection(SbVec3f *starting_position, SbVec3f *ray_direction){
	double a, b, c,d;
	SbVec3f emc ;
	float T;
	emc = (*starting_position - this->position);
	a = ray_direction->dot(*ray_direction);
	b = (2 * (*ray_direction)).dot(emc);
	c = emc.dot(emc) - (this->radius * this->radius);
	d = calculate_determinant(a, b, c);
	if(d == -1)
		return false;
	else{
	    T = calculate_solution(d,b,a);
	    if(T != -1)
		  return true;
        else
          return false;
	}
}
