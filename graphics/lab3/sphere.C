#include "sphere.h"

Sphere::Sphere(){
	position.setValue(0.0,0.0,0.0);
	radius = 1;
	this->setCoefficients(1,1,1,0,0,0,0,0,0,-1);
}
Sphere::Sphere ( OSUObjectData * obj) {
    isShiny = false;
    isTransparent = false;
	SoSphere * sph = (SoSphere *)obj->shape;
	radius = sph->radius.getValue();
	//radius = 1; //ntc
	material = obj->material;
    const float* s = obj->material->shininess.getValues(0);
    shininess = s[0];
    const float* t = obj->material->transparency.getValues(0);
    transparency = t[0];
	if(shininess > 0) isShiny = true;
	if(transparency > 0) isTransparent = true;
	SoTransform * transformation = obj->transformation;
	transform(transformation);
	//position.setValue(0.0,0.0,0.0); //ntc
	this->setCoefficients(1,1,1,0,0,0,0,0,0,-1);
}

void Sphere::print_details(){
	std::cout<<"Sphere Position:";
	print_vector(position);
	std::cout <<"Scale By:";
	print_vector(scale_vector);
	std::cout<<"Rotate by "<<rotation_angle<<" around axis:";
	print_vector(rotation_axis);
	std::cout<<"Translate by:";
	print_vector(translation_vector);
	std::cout<<"Diffuse Color: ";
	print_vector(material->diffuseColor[0]);

}

void Sphere::print_vector(SbVec3f vec){
	std::cout <<" ("<<vec[0]<<","<<vec[1]<<","<<vec[2]<<")"<<std::endl;
}

void Sphere::transform(SoTransform *transformation){

	scale_vector = transformation->scaleFactor.getValue();
    radius = scale_vector[0];
	SbRotation rotation = transformation->rotation.getValue();
	rotation.getValue(rotation_axis, rotation_angle);
	translation_vector = transformation->translation.getValue();
	SbMatrix T,S, R, F;
	T.setTranslate(translation_vector);
	R.setRotate(rotation);
	S.setScale(scale_vector);
//	F = T*R*S;
    F = S * R *T;
	SbVec3f pos(0,0,0);
	F.multVecMatrix(pos, pos);
	M = F;
	if(M.det4() != 0 ){
	    FILE *fp = fopen("testfile", "a");
	    std::cout << "Storing inverse"<< M.det4() << std::endl;
	    M.print(fp);
        iM = F.inverse();
        iM.print(fp);
        fclose(fp);
        //std::cout<< iM;
    }
	position = pos;
    print_vector(position);
}
/*
bool Sphere::intersection(SbVec3f *starting_position, SbVec3f *ray_direction, float* T){
	double a, b, c,d;
	SbVec3f emc ;
	emc = (*starting_position - this->position);
	a = ray_direction->dot(*ray_direction);
	b = (2 * (*ray_direction)).dot(emc);
	c = emc.dot(emc) - (this->radius * this->radius);
	d = calculate_determinant(a, b, c);

	if(d == -1)
		return false;

	else{
	    //std::cout<<" "<<d;
	    *T = calculate_solution(d,b,a);
	    //std::cout << " " <<*T<< std::endl;
	    if(*T != -1)
		  return true;
        else
          return false;
	}
}
*/
/*
SbVec3f Sphere::point_of_intersection (SbVec3f *starting_position, SbVec3f *ray_direction, float T){
	double a, b, c,d;
	SbVec3f sp = *starting_position ;
	SbVec3f rd = *ray_direction;
	SbVec3f poi;

	//sp = multiply_with_inverse(sp);
	//rd = multiply_with_inverse(rd);
	//rd.normalize();
	poi= sp + (T * (rd));
	//poi = this->multiply_with_transformation(poi); //ntc
	return poi;
}
*/
SbVec3f Sphere::point_of_intersection(SbVec3f *starting_position, SbVec3f *ray_direction, float T){
	double a, b, c,d;
	SbVec3f sp = *starting_position ;
	SbVec3f rd = *ray_direction;
	SbVec3f origin(0.0, 0.0, 0.0);
	SbVec3f poi;

	sp = multiply_with_inverse(sp);
	rd = multiply_with_inverse(rd);
	origin = multiply_with_inverse(origin);
    rd = rd - origin;

	rd.normalize();
	poi= sp + (T * (rd));
	poi = this->multiply_with_transformation(poi); //ntc
	return poi;
}



double Sphere::calculate_determinant(double a, double b, double c){
	double d = 0;
        d = (b*b) - (4*a*c);
        if(d < 0) return -1;
        else return sqrt(d);
}

double Sphere::calculate_solution(double d, double b, double a){
	double sol1, sol2;
        if(d == 0)
                return ((-b)/(2 * a));

        sol1 = (((-b)+d)/(2 * a));
        sol2 = (((-b)-d)/(2 * a));
        if(sol1 < 0 && sol2 < 0)
         return -1;
        if(sol1 < sol2)
         return sol1;
        else
         return sol2;

}

SbVec3f Sphere::calculate_normal(SbVec3f *starting_position, SbVec3f *ray_direction, float t){
    SbVec3f normal;
    SbVec3f sp = *starting_position ;
	SbVec3f rd = *ray_direction;
	SbVec3f origin(0.0,0.0,0.0);
	//normal = (sp + (t *(rd))) - position;
	//print_vector(normal); // 1
	//normal = (sp + (t *(rd))) -  multiply_with_transformation(origin);
	//print_vector(normal); // 2
	sp = multiply_with_inverse(sp);
	rd = multiply_with_inverse(rd);
	origin = multiply_with_inverse(origin);
	rd = rd - origin;
	rd.normalize();

    normal = (sp + (t *(rd))); //ntc
    //print_vector(normal); //3
    //normal = (*starting_position + (t *(*ray_direction))) - position; // -  multiply_with_transformation(origin);//
    return normal;
}
/*
SbVec3f Sphere::calculate_normal(SbVec3f *starting_position, SbVec3f *ray_direction, float t){
    SbVec3f normal;
    //SbVec3f sp = *starting_position ;
	//SbVec3f rd = *ray_direction;
	//sp = multiply_with_inverse(sp);
	//rd = multiply_with_inverse(rd);
	//rd.normalize();
    //normal = (sp+ (t *(rd)));// - position; //ntc
    //normal = multiply_with_transformation(normal);
    normal = (*starting_position + (t *(*ray_direction))) - position;
    return normal;
}
*/
//SbVec3f Sphere::calculate_point_of_intersection(SbVec3f *starting_position, SbVec3f *ray_direction, float t){
  //  SbVec3f normal;
   // normal = (*starting_position + (t *(*ray_direction))) - position;
   // return normal;
//}
