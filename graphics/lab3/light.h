#include "common.h"

class Light {
 public:
	SbVec3f position;
	SbVec3f color;
	float intensity;
	Light();
	Light(SbVec3f pos, float intens, SbVec3f clr);
	void print_details();
};
