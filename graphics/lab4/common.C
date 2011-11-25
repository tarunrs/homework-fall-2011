#include "common.h"


void print_vector(SbVec3f vec){
	std::cout <<" ("<<vec[0]<<","<<vec[1]<<","<<vec[2]<<")"<<std::endl;
}

float get_random_number(){
    float num;
    num = rand()/(float)(RAND_MAX+1);
    //num = -1 * num;
    return num;
}

void swap(float *a, float *b){
    float temp;
    temp = *a;
    *a = *b;
    *b = temp;

}

float calculate_determinant(float a, float b, float c){
	float d = 0;
        d = (b*b) - (4*a*c);
        if(d < 0) return -1;
        else return sqrt(d);
}

float calculate_solution(float d, float b, float a){
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

struct perlin
{
  int p[512];
  perlin(void);
  static perlin & getInstance(){static perlin instance; return
instance;}
};

static int
permutation[] = { 151,160,137,91,90,15,
  131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
  190,6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
  88,237,149,56,87,174,20,125,136,171,168,68,175,74,165,71,134,139,48,27,166,
  77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
  102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208,89,18,169,200,196,
  135,130,116,188,159,86,164,100,109,198,173,186,3,64,52,217,226,250,124,123,
  5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
  23,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167,43,172,9,
  129,22,39,253,19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
  251,34,242,193,238,210,144,12,191,179,162,241,81,51,145,235,249,14,239,107,
  49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127,4,150,254,
  138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
};

static double fade(double t)
{
  return t * t * t * (t * (t * 6 - 15) + 10);
}

static double lerp(double t, double a, double b) {
  return a + t * (b - a);
}

static double grad(int hash, double x, double y, double z) {
  int h = hash & 15;
  // CONVERT LO 4 BITS OF HASH CODE
  double u = h<8||h==12||h==13 ? x : y, // INTO 12 GRADIENT DIRECTIONS.
  v = h < 4||h == 12||h == 13 ? y : z;
  return ((h & 1) == 0 ? u : -u) + ((h&2) == 0 ? v : -v);
}

double noise(double x, double y, double z) {
  perlin & myPerlin = perlin::getInstance();
  int X = (int)floor(x) & 255, // FIND UNIT CUBE THAT
      Y = (int)floor(y) & 255, // CONTAINS POINT.
      Z = (int)floor(z) & 255;
  x -= floor(x);                   // FIND RELATIVE X,Y,Z
  y -= floor(y);                   // OF POINT IN CUBE.
  z -= floor(z);
  double u = fade(x),              // COMPUTE FADE CURVES
         v = fade(y),              // FOR EACH OF X,Y,Z.
         w = fade(z);
  int A = myPerlin.p[X]+Y,    // HASH COORDINATES OF
      AA = myPerlin.p[A]+Z,   // THE 8 CUBE CORNERS,
      AB = myPerlin.p[A+1]+Z,
      B = myPerlin.p[X+1]+Y,
      BA = myPerlin.p[B]+Z,
      BB = myPerlin.p[B+1]+Z;

  return
    lerp(w, lerp(v, lerp(u, grad(myPerlin.p[AA], x, y, z),      // AND ADD
                           grad(myPerlin.p[BA], x-1, y, z)),    // BLENDED
                   lerp(u, grad(myPerlin.p[AB], x, y-1, z),     // RESULTS
                           grad(myPerlin.p[BB], x-1, y-1, z))), // FROM 8
           lerp(v, lerp(u, grad(myPerlin.p[AA+1], x, y, z-1),   // CORNERS
                           grad(myPerlin.p[BA+1], x-1, y, z-1)),// OF CUBE
                   lerp(u, grad(myPerlin.p[AB+1], x, y-1, z-1 ),
                           grad(myPerlin.p[BB+1], x-1, y-1, z-1 ))));
}

perlin::perlin (void)
{
  for (int i=0; i < 256 ; i++) {
    p[256+i] = p[i] = permutation[i];
}
}

SbVec3f offset_normal(SbVec3f normal, float bump_by ){

    float noix, noiy, noiz;
    noix = noise(normal[0], normal[1], normal[2]);
    noiy = noise( normal[1], normal[2], normal[0]);
    noiz = noise(normal[2], normal[0], normal[1]);
    print_vector(normal);
    normal[0] = (1.0f - bump_by) * normal[0] + bump_by * noix;
    normal[1] = (1.0f - bump_by) * normal[1] + bump_by * noiy;
    normal[2] = (1.0f - bump_by) * normal[2] + bump_by * noiz;
    print_vector(normal);
    float temp = normal.dot(normal);
    if(temp == 0.0f) return normal;
    //temp = invsqrtf(temp);
    temp = sqrt(temp);
    temp = 1.0f/temp;

    normal = temp * normal;
    print_vector(normal);
    std::cout<<temp<<std::endl<<std::endl;
    return normal;
}
