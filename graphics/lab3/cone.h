#include "common.h"
#include "object.h"

class Cone : public Object{
    public:
    Cone();
    Cone(float A, float B, float C);
    SbVec3f calculate_normal(SbVec3f *starting_position, SbVec3f *ray_direction, float t);
};
