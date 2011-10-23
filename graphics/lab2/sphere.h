#include "common.h"

class Sphere{
 public:
	SbVec3f position;
	float radius;
	float shininess;
	SbVec3f scale_vector;
	SbVec3f translation_vector;
	SbVec3f rotation_axis;
	float rotation_angle;
	SoMaterial * material;
	Sphere();
	Sphere ( OSUObjectData * obj) ;
	void transform(SoTransform *transformation);
	bool intersection (SbVec3f *starting_position, SbVec3f *ray_direction, float * T);
	double calculate_determinant (double a, double b, double c);
	double calculate_solution (double d, double b, double a);
	void print_details();
	void print_vector(SbVec3f vec);
	SbVec3f calculate_normal(SbVec3f *starting_position, SbVec3f *ray_direction, float t);
	SbVec3f point_of_intersection (SbVec3f *starting_position, SbVec3f *ray_direction, float T);
};
