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


/* ATTRIBUTION: The perlin noise function is directly based on the code given at
http://www.codermind.com/articles/Raytracer-in-C++-Part-III-Textures.html
Do not consider it for extra credit if you think it is wrong
*/

struct perlin
{
  int p[512];
  perlin(void);
  static perlin & getInstance(){static perlin instance; return
instance;}
};

int permutation[256] ;

void perlin_init(){
    time_t seconds;
    time(&seconds);
    srand((unsigned int)seconds);
    for(int i=0; i<256; i++){
        permutation[i] = rand() % 256;
    }
}

double scurve(double t)
{
  return t * t * t * (t * (t * 6 - 15) + 10);
}

double interpolate(double t, double a, double b) {
  return a + t * (b - a);
}

double gradient(int hash, double x, double y, double z) {
  int h = hash & 15;
  double u = h<8||h==12||h==13 ? x : y,
  v = h < 4||h == 12||h == 13 ? y : z;
  return ((h & 1) == 0 ? u : -u) + ((h&2) == 0 ? v : -v);
}

double noise(double x, double y, double z) {
    perlin & tempPerl = perlin::getInstance();
    int X = (int)floor(x) & 255;
    int Y = (int)floor(y) & 255;
    int Z = (int)floor(z) & 255;
    x -= floor(x);
    y -= floor(y);
    z -= floor(z);
    double u = scurve(x), v = scurve(y), w = scurve(z);
    int A = tempPerl.p[X]+Y;
    int AA = tempPerl.p[A]+Z;
    int AB = tempPerl.p[A+1]+Z;
    int B = tempPerl.p[X+1]+Y;
    int BA = tempPerl.p[B]+Z;
    int BB = tempPerl.p[B+1]+Z;

    double tempA = interpolate(v, interpolate(u, gradient(tempPerl.p[AA], x, y, z),
                           gradient(tempPerl.p[BA], x-1, y, z)),
                   interpolate(u, gradient(tempPerl.p[AB], x, y-1, z),
                           gradient(tempPerl.p[BB], x-1, y-1, z)));
    double tempB = interpolate(v, interpolate(u, gradient(tempPerl.p[AA+1], x, y, z-1),
                           gradient(tempPerl.p[BA+1], x-1, y, z-1)),
                   interpolate(u, gradient(tempPerl.p[AB+1], x, y-1, z-1 ),
                           gradient(tempPerl.p[BB+1], x-1, y-1, z-1 )));
  return interpolate(w, tempA, tempB);
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
    temp = sqrt(temp);
    temp = 1.0f/temp;

    normal = temp * normal;
    print_vector(normal);
    std::cout<<temp<<std::endl<<std::endl;
    return normal;
}

