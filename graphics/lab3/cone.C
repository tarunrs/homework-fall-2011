#include "cone.h"


Cone::Cone(){
    m_A = -1;
    m_B = 1;
    m_C = 1;
}


Cone::Cone(float A, float B, float C){
    m_A = A;
    m_B = B;
    m_C = C;
}

SbVec3f Cone::calculate_normal(SbVec3f *starting_position, SbVec3f *ray_direction, float t){
    SbVec3f normal;
    SbVec3f poi;
    poi = (*starting_position + (t *(*ray_direction)));
    normal[0] = -2*poi[0];
    normal[1] = 2*poi[1];
    normal[2] = 2*poi[2];
    normal.normalize();
    return normal;
}
