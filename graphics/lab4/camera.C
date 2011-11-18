#include "camera.h"

Camera::Camera(){
	position.setValue ( 0.0, 0.0, 1.0);
	view_up.setValue ( 0.0, 1.0, 0.0);
	looks_at.setValue ( 0.0, 0.0, 0.0);
	aspect_ratio = 1.0;
	FOV_angle_degrees = 45;
	FOV_angle = (FOV_angle_degrees * PI) / 180;

}
Camera::Camera(SoCamera * camera){
	position = camera->position.getValue();
	orientation = camera->orientation.getValue();
	orientation.getValue(rotation_axis, rotation_angle);
	aspect_ratio = camera->aspectRatio.getValue();
	orientation.multVec(SbVec3f(0, 0, -1), direction);
	orientation.multVec(SbVec3f(0, 1, 0), view_up);
	if(camera->getTypeId() == SoPerspectiveCamera::getClassTypeId())
		height_angle = ((SoPerspectiveCamera *)camera)->heightAngle.getValue();
}

