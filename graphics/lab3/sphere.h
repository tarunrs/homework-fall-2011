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
	//bool intersection (SbVec3f *starting_position, SbVec3f *ray_direction, float * T);
	//bool intersection1 (SbVec3f *starting_position, SbVec3f *ray_direction, float * T);
	double calculate_determinant (double a, double b, double c);
	double calculate_solution (double d, double b, double a);
	void print_details();
	void print_vector(SbVec3f vec);
	SbVec3f calculate_normal(SbVec3f *starting_position, SbVec3f *ray_direction, float t);
	SbVec3f point_of_intersection (SbVec3f *starting_position, SbVec3f *ray_direction, float T);
	//SbVec3f calculate_normal(SbVec3f *starting_position, SbVec3f *ray_direction, float t);
	//SbVec3f point_of_intersection1 (SbVec3f *starting_position, SbVec3f *ray_direction, float T);
};
