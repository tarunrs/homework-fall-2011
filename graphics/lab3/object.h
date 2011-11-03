#include "common.h"

class Object  {
    public:

    int shapeType;
    float m_A, m_B, m_C, m_D, m_E, m_F, m_G, m_H, m_I, m_J, m_K;
    float m_dx, m_dy, m_dz;
    SbMatrix M;
	SbMatrix iM;
	SbVec3f position;
	bool isShiny;
	bool isTransparent;
	float radius; // for sphere
	float height, width, depth; // for cube
	SbVec3f x1, x2, y1, y2, z1, z2, tip_position;
	float shininess;
	float transparency;
	SbVec3f scale_vector;
	SbVec3f translation_vector;
	SbVec3f rotation_axis;
	float rotation_angle;
	SoMaterial * material;
    Object();
    Object(float p_A, float  p_B, float p_C, float p_D, float p_E, float p_F, float p_G, float p_H, float p_I, float p_J);
    void setCoefficients(float p_A, float  p_B, float p_C, float p_D, float p_E, float p_F, float p_G, float p_H, float p_I, float p_J);
    bool intersection(SbVec3f *starting_position, SbVec3f *ray_direction, float * T);
    SbVec3f multiply_with_inverse(SbVec3f pt);
    SbVec3f multiply_with_transformation(SbVec3f pt);
    float calculate_determinant(float a, float b, float c);
    float calculate_solution(float d, float b, float a);
};
