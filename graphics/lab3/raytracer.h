#include "common.h"
#include "shapes.h"
#include "light.h"
#include "camera.h"

#define RI 1.5

class Pixel{
    public:
    int r,g,b;
    Pixel(int a, int x, int c);
    Pixel();
};


class RayTracer{
 public:
    float focal_length;
    FILE *fp;
	OSUInventorScene *scene;
	int shadow_on, reflection_on, refraction_on, super_sampling_on ;
	//std::vector<Sphere> spheres;
	std::vector<Object> objects;
	std::vector<Light> lights;
	std::string op_filename;
	Camera * camera;
	float image_height, image_width;
	SbVec3f image_C, image_L;
	float pixel_width, pixel_height;
	SbVec3f u,v,n;
	int x_resolution, y_resolution;
    std::vector<std::vector<Pixel> > image;
	RayTracer ( std::string ip_filename, std::string op_filename, int xresolution, int yresolution,int s_on, int r_on, int refract_on, int depth_of_field_on, int super_sampling);
	void read_open_inventor_scene(std::string iv_file);
	void read_objects();
	void read_camera();
	void read_lights();
	void calculate_eye_coordinate_system(Camera *cam);
	void trace_rays();
	bool shade(SbVec3f *ray_origin, SbVec3f *ray_direction, SbVec3f *color, int recursionDepth, int flag=0);
	bool distribute_shade(int i, int j, SbVec3f *position, SbVec3f *color);
	bool depth_of_field(int i, int j, SbVec3f *position, SbVec3f *color);
	void print_vector(SbVec3f vec);
	void calculate_image_dimentions();
	SbVec3f  calculate_pixel_location(int i, int j, float du, float dv);
	double calculate_determinant(double a, double b, double c);
	double calculate_solution(double d, double b, double a);
	void write_to_file(std::vector<std::vector<Pixel> > img);
	void open_file();
    void close_file();
    bool shadow_ray_intersection(SbVec3f *point_of_intersection, SbVec3f *ray_direction, int light_source);
    float soft_shadow_ray_intersection(SbVec3f *point_of_intersection, int light_source);
    float should_transmit(SbVec3f *ray_direction, SbVec3f *normal_at_intersection, float refractive_index);
    bool refract(SbVec3f *ray_direction, SbVec3f *normal_at_intersection, SbVec3f *T);
    SbVec3f reflect(SbVec3f *normal_at_intersection, SbVec3f *ray_direction);
    int min(float val);
    SbVec3f calculate_view_up(SbVec3f actual_ray_direction);
    void calculate_coordinate_system(SbVec3f *u, SbVec3f *v, SbVec3f *n, SbVec3f actual_ray_direction);
};

