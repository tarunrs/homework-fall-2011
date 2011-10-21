#include "common.h"

class Light {
 public: 
	SbVec3f position;
	Light();
	Light(SbVec3f pos);
	void print_details();
};
