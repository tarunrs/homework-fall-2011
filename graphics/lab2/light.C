#include "light.h"

Light::Light(){
	position.setValue(0.0,0.0,0.0);
}

Light::Light(SbVec3f pos){
	position = pos;
}
void Light::print_details(){
	std::cout<<"Light Position : "<<position[0]<<
	" , "<<position[1]<<
	" , "<<position[2]<< std::endl;
}
