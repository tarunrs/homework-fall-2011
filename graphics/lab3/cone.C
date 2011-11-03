#include "shapes.h"


Cone::Cone(){
    shapeType = 3;
    m_A = -1;
    m_B = 1;
    m_C = 1;
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


Cone::Cone(float A, float B, float C){
    shapeType = 3;
    m_A = A;
    m_B = B;
    m_C = C;
        SbMatrix T,S, R, F;
    SbRotation rotation(0.01,0.01,0.01,0.01) ;
    SbVec3f trans(0,0,0);
    SbVec3f scale(0.1,0.1, 0.1);
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

/*
SbVec3f Cone::calculate_normal(SbVec3f *starting_position, SbVec3f *ray_direction, float t){
    SbVec3f normal;
    SbVec3f poi;
    poi = (*starting_position + (t *(*ray_direction)));
    normal[0] = -1*poi[0];
    normal[1] = 1*poi[1];
    normal[2] = 1*poi[2];
    normal.normalize();
    return normal;
}
*/


// the below two function use the local coordinates to determine the point of intersection
SbVec3f Cone::point_of_intersection(SbVec3f *starting_position, SbVec3f *ray_direction, float T){
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
SbVec3f Cone::calculate_normal(SbVec3f *starting_position, SbVec3f *ray_direction, float t){
    SbVec3f poi, normal;

    poi = point_of_intersection( starting_position, ray_direction, t);
    normal[0] = -1 * poi[0];//(sp + (t *(rd))); //ntc
    normal[1] = 1 * poi[1];
    normal[2] = 1 * poi[2];
    return normal;
}

