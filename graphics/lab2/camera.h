#include "common.h"
class Camera {
 public:
	int x;
	SbVec3f position;
	SbVec3f looks_at;
	SbRotation orientation;
	SbVec3f rotation_axis;
	SbVec3f direction;
	SbVec3f view_up;
	SbVec3f U, V, N;
	float image_width, image_height;
	float rotation_angle;
	float aspect_ratio;
	float FOV_angle;
	float FOV_angle_degrees;
	float height_angle;

	Camera();
	Camera(SoCamera * camera);
};
