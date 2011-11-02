#include "common.h"
#include "object.h"
//#include "sphere.h"
//#include "cone.h"


class Cone : public Object{
    public:
    Cone();
    Cone(float A, float B, float C);
	SbVec3f calculate_normal(SbVec3f *starting_position, SbVec3f *ray_direction, float t);
	SbVec3f point_of_intersection (SbVec3f *starting_position, SbVec3f *ray_direction, float T);
};

class Sphere : public Object{
 public:
	Sphere();
	Sphere ( OSUObjectData * obj) ;
	void transform(SoTransform *transformation);
	void print_details();
	SbVec3f calculate_normal(SbVec3f *starting_position, SbVec3f *ray_direction, float t);
	SbVec3f point_of_intersection (SbVec3f *starting_position, SbVec3f *ray_direction, float T);
};
