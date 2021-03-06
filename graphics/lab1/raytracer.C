#include "raytracer.h"

RayTracer::RayTracer(std::string ip_filename, std::string opfilename, int xresolution, int yresolution){
	x_resolution = xresolution;
	y_resolution = yresolution;
	op_filename = opfilename;
	read_open_inventor_scene(ip_filename);
}

void RayTracer::read_open_inventor_scene(std::string iv_file){
	SoDB::init();
	scene = new OSUInventorScene((char *)iv_file.c_str());
	read_objects();
	read_camera();
	read_lights();
	calculate_eye_coordinate_system(camera);
	calculate_image_dimentions();
	return;
}

void RayTracer::read_lights(){
	for(int  i= 0; i <scene->Lights.getLength(); i++){
		SoLight * light = (SoLight *) scene->Lights[i];
		SoType light_type = light->getTypeId();

		if(light->on.getValue() == true){
			if(light_type == SoPointLight::getClassTypeId()){
				SoPointLight * point_light = (SoPointLight*) light;
				SbVec3f location = point_light->location.getValue();
				Light * new_light = new Light(location);
				//new_light->print_details();
				lights.push_back(*new_light);
			}
		}
	}
}
void RayTracer::calculate_image_dimentions(){
	float d = 10.0;
	image_height = 2 * d * (tan(camera->height_angle/2));
	image_width = image_height * camera->aspect_ratio;
	image_C = camera->position - ( n * d);
	print_vector(image_C);
	image_L = image_C - (u * (image_width/2)) - ( v * (image_height/2));
	print_vector(image_L);
	pixel_width = image_width/x_resolution;
	pixel_height = image_height/y_resolution;
}

SbVec3f RayTracer::calculate_pixel_location(int i, int j){
	SbVec3f pos;
	pos = image_L + (u * i * pixel_width) + ( v * j * pixel_height);
	return pos;

}


void RayTracer::calculate_eye_coordinate_system(Camera *cam){
    n= -cam->direction;
    n.normalize();
	u = cam->view_up.cross(n);
    u.normalize();
	v = n.cross(u);
    v.normalize();
}

void RayTracer::read_camera(){
	SoCamera * so_camera = (SoCamera *)scene->Camera;
	camera = new Camera(so_camera);
}

void RayTracer::read_objects(){
	    for (int i = 0; i < scene->Objects.getLength(); i++) {
                OSUObjectData *obj = (OSUObjectData *) scene->Objects[i];
                if(!obj->Check()){
                        std::cerr<< "Error reading Object "<< i <<std::endl;
                        exit(0);
                }
                Sphere * temp = new Sphere(obj);
                spheres.push_back(*temp);
        }
}

double RayTracer::calculate_determinant(double a, double b, double c){
	double d = 0;
	d = (b*b) - (4*a*c);
	if(d < 1) return -1;
	else return sqrt(d);
}

double RayTracer::calculate_solution(double d, double b, double a){
	double sol1, sol2;
	if(d == 0)
		return ((-b)/(2 * a));

	sol1 = (((-b)+d)/(2 * a));
	sol2 = (((-b)-d)/(2 * a));
	if(sol1 < sol2) return sol1;
	else return sol2;

}

bool RayTracer::shade(SbVec3f *ray_origin, SbVec3f *ray_direction, SbColor *color){
	float t_value, t_min = 999;
	SbVec3f normal_at_intersection;
	bool should_color = false;
	SbVec3f z (0.0,0.0,-1.0);
	float color_scale;
    double r_comp, g_comp,b_comp;


    for(int k =0; k<spheres.size(); k++){
        Sphere temp = spheres.at(k);

        if(temp.intersection(ray_origin, ray_direction, &t_value))
        {
            if(t_value < t_min) {
                t_min = t_value;
                normal_at_intersection = temp.calculate_normal(ray_origin, ray_direction, t_value);
                normal_at_intersection.normalize();
                color_scale = normal_at_intersection.dot(z);
                r_comp = fabs(color_scale * temp.material->diffuseColor[0][0]);
                g_comp = fabs(color_scale  * temp.material->diffuseColor[0][1]);
                b_comp = fabs(color_scale * temp.material->diffuseColor[0][2]);

                color->setValue(r_comp*255, g_comp*255, b_comp*255);
                should_color = true;
            }
        }
    }

    return should_color;
}

void RayTracer::trace_rays(){
	int i,j;
	SbVec3f pix_pos, d_vec;
	double a,b,c;
	float color_scale;
	SbColor color;
    std::vector<Pixel> image_row;
	std::cout << "Tracing rays"<<std::endl;

	for (i=0; i < y_resolution; i++){
        image_row.clear();
	    for (j=0; j < x_resolution; j++) {
            pix_pos = calculate_pixel_location(i,j);
            d_vec  = pix_pos - camera->position;

            bool should_color = shade(&(camera->position), &d_vec, &color);
            if(should_color)
            {
                //r_comp = fabs(color_scale * temp.material->diffuseColor[0][0]);
                //g_comp = fabs(color_scale  * temp.material->diffuseColor[0][1]);
                //b_comp = fabs(color_scale * temp.material->diffuseColor[0][2]);

                image_row.push_back(Pixel(min(color[0]), min(color[1]), min(color[2])));
            }
            else
                image_row.push_back(Pixel(55,55,55));

        }
        image.push_back(image_row);
	}
	open_file();
	write_to_file(image);
	close_file();
	std::cout<<"Done Tracing"<<std::endl;
	return;
}
/*
void  RayTracer::shade(float* ray_origin, float* ray_direction, float *color){
 return;
}
*/
void RayTracer::print_vector(SbVec3f vec){
	std::cout<<"( "
		<< vec[0]<<","
		<< vec[1]<<","
		<< vec[2]<<" )"<<std::endl;
}

void RayTracer::open_file(){
     fp = fopen(op_filename.c_str(),"w");
     int height = x_resolution;
     int width = y_resolution;
     int max_ccv = 255;
     fprintf(fp,"P3 %d %d %d\n", width, height, max_ccv);
}

void RayTracer::close_file(){
             fclose(fp);
}

void RayTracer::write_to_file(std::vector<std::vector<Pixel> > img){
    Pixel temp;
    for(int i=x_resolution; i>=0;i--){
        for (int j = 0; j < y_resolution; j++) {
                temp = img[j][i];
                fprintf(fp,"%d %d %d ", temp.r, temp.g , temp.b);
        }
    }
}

int RayTracer::min(float val){
    return (val > 255 ? 255 : floor(val));

}

Pixel::Pixel(int a, int x, int c){
    r = a;
    g = x;
    b = c;
}
Pixel::Pixel(){
    r = 0;
    g = 0;
    b = 0;
}
