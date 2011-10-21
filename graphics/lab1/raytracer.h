#include "common.h"
#include "sphere.h"
#include "light.h"
#include "camera.h"
class Pixel{
    public:
    int r,g,b;
    Pixel(int a, int x, int c);
    Pixel();
};


class RayTracer{
 public:
    FILE *fp;
	OSUInventorScene *scene;
	std::vector<Sphere> spheres;
	std::vector<Light> lights;
	std::string op_filename;
	Camera * camera;
	float image_height, image_width;
	SbVec3f image_C, image_L;
	float pixel_width, pixel_height;
	SbVec3f u,v,n;
	int x_resolution, y_resolution;
    std::vector<std::vector<Pixel> > image;
	RayTracer ( std::string ip_filename, std::string op_filename, int xresolution, int yresolution);
	void read_open_inventor_scene(std::string iv_file);
	void read_objects();
	void read_camera();
	void read_lights();
	void calculate_eye_coordinate_system(Camera *cam);
	void trace_rays();
	bool shade(SbVec3f *ray_origin, SbVec3f *ray_direction, SbColor *color);
	//bool RayTracer::shade(SbVec3f *ray_origin, SbVec3f *ray_direction, SbColor *color){
	//void shade(float* ray_origin, float* ray_direction, float *color);
	void print_vector(SbVec3f vec);
	void calculate_image_dimentions();
	SbVec3f  calculate_pixel_location(int i, int j);
	double calculate_determinant(double a, double b, double c);
	double calculate_solution(double d, double b, double a);
	void write_to_file(std::vector<std::vector<Pixel> > img);
	void open_file();
    void close_file();
    int min(float val);
};

