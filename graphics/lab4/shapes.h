#include "common.h"
#include "object.h"
//#include "sphere.h"
//#include "cone.h"


class Cone : public Object{
    public:
    Cone();
    Cone(float X, float Y, float Z);
    Cone ( OSUObjectData * obj) ;
    Cone (const Object& b);
	void transform(SoTransform *transformation);
    bool intersection (SbVec3f *starting_position, SbVec3f *ray_direction, float* T);
	SbVec3f calculate_normal(SbVec3f *starting_position, SbVec3f *ray_direction, float t);
	SbVec3f point_of_intersection (SbVec3f *starting_position, SbVec3f *ray_direction, float T);
};

class Cylinder : public Object{
    public:
    Cylinder();
    Cylinder(float X, float Y, float Z);
    Cylinder( OSUObjectData * obj) ;
    Cylinder(const Object& b);
	void transform(SoTransform *transformation);
    bool intersection (SbVec3f *starting_position, SbVec3f *ray_direction, float* T);
	SbVec3f calculate_normal(SbVec3f *starting_position, SbVec3f *ray_direction, float t);
	SbVec3f point_of_intersection (SbVec3f *starting_position, SbVec3f *ray_direction, float T);
};

class Sphere : public Object{
 public:
	Sphere();
	Sphere ( OSUObjectData * obj) ;
	Sphere (const Object& b);
	void translate(SbVec3f trans);
	Sphere(float rad, SbVec3f pos);
	void transform(SoTransform *transformation);
	void print_details();
	//bool intersection (SbVec3f *starting_position, SbVec3f *ray_direction, float* T);
	SbVec3f calculate_normal(SbVec3f *starting_position, SbVec3f *ray_direction, float t);
	SbVec3f point_of_intersection (SbVec3f *starting_position, SbVec3f *ray_direction, float T);
};

class Cube: public Object{
    public:
    Cube();
    //Cube(float A, float B, float C);
    Cube ( OSUObjectData * obj) ;
    Cube (const Object& b);
	void transform(SoTransform *transformation);
	void print_details();
	bool intersection(SbVec3f *starting_position, SbVec3f *ray_direction, float * T);
	SbVec3f calculate_normal(SbVec3f *starting_position, SbVec3f *ray_direction, float t);
	SbVec3f point_of_intersection (SbVec3f *starting_position, SbVec3f *ray_direction, float T);
};
