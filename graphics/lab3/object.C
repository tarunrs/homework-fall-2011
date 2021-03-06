#include "object.h"

Object::Object(){
    m_A = 0.0;
    m_B = 0.0;
    m_C = 0.0;
    m_D = 0.0;
    m_E = 0.0;
    m_F = 0.0;
    m_G = 0.0;
    m_H = 0.0;
    m_I = 0.0;
    m_J = 0.0;
    m_K = 1.0;
}

Object::Object(float p_A, float  p_B, float p_C, float p_D, float p_E, float p_F, float p_G, float p_H, float p_I, float p_J){
    m_A = p_A;
    m_B = p_B;
    m_C = p_C;
    m_D = p_D;
    m_E = p_E;
    m_F = p_F;
    m_G = p_G;
    m_H = p_H;
    m_I = p_I;
    m_J = p_J;
}


void Object::setCoefficients(float p_A, float  p_B, float p_C, float p_D, float p_E, float p_F, float p_G, float p_H, float p_I, float p_J){
    m_A = p_A;
    m_B = p_B;
    m_C = p_C;
    m_D = p_D;
    m_E = p_E;
    m_F = p_F;
    m_G = p_G;
    m_H = p_H;
    m_I = p_I;
    m_J = p_J;
}

/*
Object::Object(float p_A, float  p_B, float p_C){
    m_A = p_A;
    m_B = p_B;
    m_C = p_C;
}
*/
bool Object::intersection(SbVec3f *starting_position, SbVec3f *ray_direction, float * T){
    float a, b, c, d;
    float xd, yd, zd;
    float xr, yr, zr;
    SbVec3f rd = *ray_direction;
    SbVec3f sp = *starting_position;
    SbVec3f origin(0.0,0.0,0.0);

    rd = multiply_with_inverse(rd);
    origin = multiply_with_inverse(origin);
    rd = rd - origin;
    rd.normalize();
    sp = multiply_with_inverse(sp);

    xd = rd[0];
    yd = rd[1];
    zd = rd[2];

    xr = sp[0];
    yr = sp[1];
    zr = sp[2];

    a = (m_A * pow(xd, 2)) + (m_B * pow(yd, 2)) + (m_C * pow(zd, 2))
        + (m_D* xd * yd) + (m_E * xd * zd) + (m_F * yd * zd);

    b = (2*m_A* xr * xd )+ (2* m_B * yr * yd) + (2 * m_C * zr * zd )
        + m_D * (xr * yd + yr * xd) + (m_E * xr * zd )+ m_F * (yr * zd + yd * zr)
        + (m_G * xd) + (m_H * yd) + (m_I * zd);

    c = (m_A * pow(xr,2)) + (m_B * pow(yr,2) )+ (m_C * pow(zr, 2))
        + (m_D * xr * yr) + (m_E * xr * zr) + (m_F * yr * zr)
        + (m_G * xr) + (m_H * yr) + (m_I * zr) + m_J ;

    d = calculate_determinant(a,b,c);

    if(d == -1)
        return false;
    float sol = calculate_solution(d, b, a);

    *T = sol;

    if(sol == -1)
        return false;
    else
        return true;
}

SbVec3f Object::multiply_with_inverse(SbVec3f pt){
    SbVec3f des;
    iM.multVecMatrix(pt, des);
    return des;
}

SbVec3f Object::multiply_with_transformation(SbVec3f pt){
    SbVec3f des;
    M.multVecMatrix(pt, des);
    return des;
}

float Object::calculate_determinant(float a, float b, float c){
	float d = 0;
        d = (b*b) - (4*a*c);
        if(d < 0) return -1;
        else return sqrt(d);
}

float Object::calculate_solution(float d, float b, float a){
	float sol1, sol2;
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
