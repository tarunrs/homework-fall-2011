#include "common.h"

class Light {
 public:
	SbVec3f position;
	SbVec3f color;
	float intensity;
	float radius;
	Light();
	Light(SbVec3f pos, float intens, SbVec3f clr, float rad);
	bool intersection(SbVec3f *starting_position, SbVec3f *ray_direction);
	void print_details();
};
