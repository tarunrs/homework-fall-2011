#include "common.h"

class Object  {
    public:
    float m_A, m_B, m_C, m_D, m_E, m_F, m_G, m_H, m_I, m_J, m_K;
    float m_dx, m_dy, m_dz;
    Object();
    Object(float p_A, float  p_B, float p_C, float p_D, float p_E, float p_F, float p_G, float p_H, float p_I, float p_J);
    void setCoefficients(float p_A, float  p_B, float p_C, float p_D, float p_E, float p_F, float p_G, float p_H, float p_I, float p_J);
    bool intersection(SbVec3f *starting_position, SbVec3f *ray_direction, float * T);
    float calculate_determinant(float a, float b, float c);
    float calculate_solution(float d, float b, float a);
};
