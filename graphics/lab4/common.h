#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <fstream>
#include <sstream>
#include <Inventor/SbRotation.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoLight.h>
#include <Inventor/nodes/SoPointLight.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <vector>
#include "OSUInventor.h"
#define PI 3.14159265
#define DISK_SIZE 30
//#define DISK_SIZE .75
#define NUMBER_OF_CAMERAS 200
#define NUMBER_OF_SAMPLES 20
#define NUMBER_OF_SHADOW_RAYS 200

#define TEXTURE_STRIPS 1
#define TEXTURE_CHECKERBOARD 2
#define TEXTURE_SMOOTH_COLORS 3
#define TEXTURE_CONCENTRIC_CIRCLES 4
#define TEXTURE_WOOD_GRAIN 5
#define TEXTURE_RANDOM_SURFACE 6
#define TEXTURE_CUSTOM1 7
#define TEXTURE_CUSTOM2 8
#define TEXTURE_CUSTOM3 9



void print_vector(SbVec3f vec);
float get_random_number();
void swap(float *a, float *b);
float calculate_determinant(float a, float b, float c);
float calculate_solution(float d, float b, float a);
double noise(double x, double y, double z) ;
SbVec3f offset_normal(SbVec3f normal, float bump_by);
void perlin_init();
