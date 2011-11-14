#include <iostream>
#include <stdlib.h>
#include <math.h>
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
#define NUMBER_OF_CAMERAS 100
#define NUMBER_OF_SAMPLES 16
#define NUMBER_OF_SHADOW_RAYS 200


void print_vector(SbVec3f vec);
float get_random_number();
void swap(float *a, float *b);
float calculate_determinant(float a, float b, float c);
float calculate_solution(float d, float b, float a);
