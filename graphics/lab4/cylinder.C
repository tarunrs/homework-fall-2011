#include "shapes.h"


Cylinder::Cylinder(){
    shapeType = 4;
    m_A = 1;
    m_B = 0;
    m_C = 1;
    m_J = -1;
    SbMatrix T,S, R, F;
    SbRotation rotation(0.01,0.01,0.01,0.01) ;
    SbVec3f vec(1.0,1.5, 0.5);
    SbVec3f vec1(0.5,0.5, 0.5);
	T.setTranslate(vec1);
	R.setRotate(rotation);
	S.setScale(vec);
//	F = T*R*S;
    F = S * R *T;
	SbVec3f pos(0,0,0);
	F.multVecMatrix(pos, pos);
	M = F; // save transformation matrix
	if(M.det4() != 0 ){
	    iM = F.inverse(); // store inverse if inverse exists
    }
}

Cylinder::Cylinder( OSUObjectData * obj) {
    std::cout<<"Cylinder Contructing"<<std::endl;
    shapeType = 4;
    //m_A = 1;
    //m_B = -1;
    //m_C = 1;
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
	//this->setCoefficients(1,1,1,0,0,0,0,0,0,-1);
	this->setCoefficients(1,0,1,0,0,0,0,0,0,-1);
}

void Cylinder::transform(SoTransform *transformation){

	scale_vector = transformation->scaleFactor.getValue();
    radius = scale_vector[0];

	SbRotation rotation = transformation->rotation.getValue();
	rotation.getValue(rotation_axis, rotation_angle);
	translation_vector = transformation->translation.getValue();
	SbMatrix T,S, R, F;
	T.setTranslate(translation_vector);
	print_vector(translation_vector); //ntc
	print_vector(scale_vector); //ntc
	//std::cout << rotation[0] << rotation[1]<<rotation[2]<<rotation[3]; //ntc
	print_vector(rotation_axis); //ntc
	std::cout<<rotation_angle<<std::endl<<std::endl;

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
	print_vector(position);
}

Cylinder::Cylinder(const Object& b){
    shapeType = 4;
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
     texture = b.texture;
   };

Cylinder::Cylinder(float A, float B, float C){
    shapeType = 4;
    m_A = A;
    m_B = B;
    m_C = C;
        SbMatrix T,S, R, F;
    SbRotation rotation(0.01,0.01,0.01,0.01) ;
    SbVec3f trans(0,0,0);
    SbVec3f scale(0.1, 0.1, 0.1);
	T.setTranslate(trans);
	R.setRotate(rotation);
	S.setScale(scale);
//	F = T*R*S;
    F = S * R *T;
	SbVec3f pos(0,0,0);
	F.multVecMatrix(pos, pos);
	M = F; // save transformation matrix
	if(M.det4() != 0 ){
	    iM = F.inverse(); // store inverse if inverse exists
    }
}


// the below two function use the local coordinates to determine the point of intersection
SbVec3f Cylinder::point_of_intersection(SbVec3f *starting_position, SbVec3f *ray_direction, float T){
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
SbVec3f Cylinder::calculate_normal(SbVec3f *starting_position, SbVec3f *ray_direction, float t){
    SbVec3f poi, normal;

    poi = point_of_intersection( starting_position, ray_direction, t);
    normal[0] = 1 * poi[0];//(sp + (t *(rd))); //ntc
    normal[1] = 0 * poi[1];
    normal[2] = 1 * poi[2];
    return normal;
}

bool Cylinder::intersection(SbVec3f *starting_position, SbVec3f *ray_direction, float * T){
    float temp_t;
    float bound_min = -0.5, bound_max = 0.5;
    //std::cout<<"entering";
    bool intersects = this->intersection_copy(starting_position, ray_direction, &temp_t);
    if(intersects){
        SbVec3f poi = point_of_intersection(starting_position,ray_direction, temp_t);
        //print_vector(poi);
        if(poi[1] > bound_min && poi[1] <bound_max){
        // means it is within our ymin and ymax
            *T = temp_t;
            return true;
        }
    }
    *T = -1;
    return false;
}
