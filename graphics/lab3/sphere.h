#include "common.h"
#include "object.h"

class Sphere : public Object{
 public:
	SbVec3f position;
	bool isShiny;
	bool isTransparent;
	float radius;
	float shininess;
	float transparency;
	SbVec3f scale_vector;
	SbVec3f translation_vector;
	SbVec3f rotation_axis;
	float rotation_angle;

	SoMaterial * material;
	Sphere();
	Sphere ( OSUObjectData * obj) ;
	void transform(SoTransform *transformation);
	void print_details();
	SbVec3f calculate_normal(SbVec3f *starting_position, SbVec3f *ray_direction, float t);
	SbVec3f point_of_intersection (SbVec3f *starting_position, SbVec3f *ray_direction, float T);
};
