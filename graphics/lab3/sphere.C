#include "shapes.h"

Sphere::Sphere(){
    shapeType = 1;
	position.setValue(0.0,0.0,0.0);
	radius = 1;
	this->setCoefficients(1,1,1,0,0,0,0,0,0,-1);
}
Sphere::Sphere ( OSUObjectData * obj) {
    shapeType = 1;
    isShiny = false;
    isTransparent = false;
	SoSphere * sph = (SoSphere *)obj->shape;
	radius = sph->radius.getValue();
	material = obj->material;
    const float* s = obj->material->shininess.getValues(0);
    shininess = s[0];
    const float* t = obj->material->transparency.getValues(0);
    transparency = t[0];
	if(shininess > 0) isShiny = true;
	if(transparency > 0) isTransparent = true;
	SoTransform * transformation = obj->transformation;
	transform(transformation);
	this->setCoefficients(1,1,1,0,0,0,0,0,0,-1);
}

Sphere::Sphere (const Object& b){
    shapeType = 1;
     m_A = b.m_A;
     m_B = b.m_B;
     m_C = b.m_C;
     m_D = b.m_D;
     m_E = b.m_E;
     m_F = b.m_F;
     m_G = b.m_G;
     m_H = b.m_H;
     m_I = b.m_I;
     m_J = b.m_J;
     m_K = b.m_K;
     m_dx = b.m_dx;
     m_dy = b.m_dy;
     m_dz = b.m_dz;
     iM = b.iM;
     M = b.M;
     position = b.position;
     isShiny = b.isShiny;
     isTransparent = b.isTransparent;
     radius = b.radius;
     shininess = b.shininess;
     transparency = b.transparency;
     scale_vector = b.scale_vector;
     translation_vector = b.translation_vector;
     rotation_axis = b.rotation_axis;
     rotation_angle = b.rotation_angle;
     material = b.material;
   };

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
	M = F; // save transformation matrix
	if(M.det4() != 0 ){
	    iM = F.inverse(); // store inverse if inverse exists
    }
	position = pos;
}

// the below two function use the local coordinates to determine the point of intersection
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


// this function calculates the normal using the local coordinates;
SbVec3f Sphere::calculate_normal(SbVec3f *starting_position, SbVec3f *ray_direction, float t){
    SbVec3f normal;
    SbVec3f sp = *starting_position ;
	SbVec3f rd = *ray_direction;
	SbVec3f origin(0.0,0.0,0.0);

	sp = multiply_with_inverse(sp);
	rd = multiply_with_inverse(rd);
	origin = multiply_with_inverse(origin);
	rd = rd - origin;
	rd.normalize();

    normal = (sp + (t *(rd))); //ntc
    //normal =multiply_with_transformation(normal);
    return normal;
}
