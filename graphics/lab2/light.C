#include "light.h"

Light::Light(){
	position.setValue(0.0,0.0,0.0);
}

Light::Light(SbVec3f pos, float intens, SbVec3f clr){
	position = pos;
	intensity = intens;
	color = clr;
}
void Light::print_details(){
	std::cout<<"Light Position : "<<position[0]<<
	" , "<<position[1]<<
	" , "<<position[2]<< std::endl;
}
