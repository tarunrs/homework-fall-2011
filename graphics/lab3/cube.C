#include "shapes.h"

Cube::Cube(){
	position.setValue(0.0,0.0,0.0);
	height = 1;
	depth = 1;
	width = 1;
	this->setCoefficients(0,0,0,0,0,0,0,0,0,0);
	shapeType = 2;
}/*
Cube::Cube( OSUObjectData * obj) {
    shapeType = 2;
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
*/
Cube::Cube ( OSUObjectData * obj) {
    shapeType = 2;
    isShiny = false;
    isTransparent = false;
    SoCube * cube = (SoCube *) obj->shape;
	//SoSphere * sph = (SoSphere *)obj->shape;
	//radius = sph->radius.getValue();
	width = cube->width.getValue();
	height = cube->height.getValue();
	depth = cube->depth.getValue();
	std::cout<< "setting values"<<std::endl;
	x1.setValue (0.0, 0.0, 0.0);
	x2.setValue (height, 0.0, 0.0);
	y1.setValue (0.0, 0.0, 0.0);
	y2.setValue (0.0, width, 0.0);
	z1.setValue (0.0, 0.0, 0.0);
	z2.setValue (0.0, 0.0, depth);

	tip_position.setValue(height, width, depth);
	print_vector(tip_position);
	std::cout<<"tip position";
	material = obj->material;
    const float* s = obj->material->shininess.getValues(0);
    shininess = s[0];
    const float* t = obj->material->transparency.getValues(0);
    transparency = t[0];
	if(shininess > 0) isShiny = true;
	if(transparency > 0) isTransparent = true;
	SoTransform * transformation = obj->transformation;
	transform(transformation);
	this->setCoefficients(0,0,0,0,0,0,0,0,0,0);
}

Cube::Cube (const Object& b){
     shapeType = 2;
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
     x1 = b.x1;
     x2 = b.x2;
     y1 = b.y1;
     y2 = b.y2;
     z1 = b.z1;
     z2 = b.z2;
     position = b.position;
     isShiny = b.isShiny;
     isTransparent = b.isTransparent;
     radius = b.radius;
     width = b.width;
     height = b.height;
     depth = b.depth;
     shininess = b.shininess;
     transparency = b.transparency;
     scale_vector = b.scale_vector;
     translation_vector = b.translation_vector;
     rotation_axis = b.rotation_axis;
     rotation_angle = b.rotation_angle;
     material = b.material;
     tip_position = b.tip_position;
   };

void Cube::print_details(){
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

void Cube::transform(SoTransform *transformation){

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
	SbVec3f pos(0.0 ,0.0 ,0.0);
	F.multVecMatrix(pos, pos);
	M = F; // save transformation matrix
	if(M.det4() != 0 ){
	    iM = F.inverse(); // store inverse if inverse exists
    }
	position = pos;
}

// the below two function use the local coordinates to determine the point of intersection
SbVec3f Cube::point_of_intersection(SbVec3f *starting_position, SbVec3f *ray_direction, float T){
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
SbVec3f Cube::calculate_normal(SbVec3f *starting_position, SbVec3f *ray_direction, float t){
    SbVec3f normal;
    /*SbVec3f sp = *starting_position ;
	SbVec3f rd = *ray_direction;
	SbVec3f origin(0.0,0.0,0.0);

	sp = multiply_with_inverse(sp);
	rd = multiply_with_inverse(rd);
	origin = multiply_with_inverse(origin);
	rd = rd - origin;
	rd.normalize();

    normal = (sp + (t *(rd))); //ntc
    return normal;*/

    switch(plane_of_intersection){

    case 0: normal.setValue(-1.0, 0.0, 0.0);
            //normal =  -1 * u;
            break;
    case 1: normal.setValue(0.0, -1.0, 0.0);
            //normal =  -1 * v;
            break;
    case 2: normal.setValue(0.0, 0.0, -1.0);
            //normal =  -1 * n;
            break;

    case 3: normal.setValue(1.0, 0.0, 0.0);
            //normal =  u;
            break;
    case 4: normal.setValue(0.0, 1.0, 0.0);
            //normal =  v;
            break;
    case 5: normal.setValue(0.0, 0.0, 1.0);
            //normal =  n;
            break;

    }
    return normal;
}


bool Cube::intersection(SbVec3f *starting_position, SbVec3f *ray_direction, float * T){
    float tnear, tfar, t1, t2;
    tnear = -999;
    tfar = 999;
    SbVec3f pos;
    SbVec3f p = *starting_position;
    SbVec3f D = *ray_direction;

    SbVec3f origin(0.0,0.0,0.0);

	p = multiply_with_inverse(p);
	D = multiply_with_inverse(D);
	origin = multiply_with_inverse(origin);
	D = D - origin;
	D.normalize();

    pos.setValue(0.0 ,0.0 ,0.0);
    //print_vector(tip_position);
    for(int i=0; i< 3; i++){
        if(D[i] == 0){
            print_vector(D);
            if(p[i] < pos[i] || p[i] > tip_position[i]);
                return false;
        }
        else{
            t1 = (pos[i] - p[i])/ D[i];
            t2 = (tip_position[i] - p[i])/ D[i];
      //      std::cout<<std::endl<<t1<< " "<<t2<<std::endl;
            if(t1 > t2) {plane_of_intersection = 3; swap(&t1, &t2);}
        //    std::cout<<t1<< " "<<t2<<std::endl;
            if(t1 > tnear) { tnear = t1; plane_of_intersection += i;}
            if(t2 < tfar) tfar = t2;
            if (tnear > tfar) {
                //print_vector(D);
                return false;}
            if(tfar < 0) {
                //print_vector(D);;
                return false;}
        }
    }

    *T = tnear;
    //std::cout<<"asd"<<std::endl;
    return true;

}

