#include "raytracer.h"

RayTracer::RayTracer(std::string ip_filename, std::string opfilename, int xresolution, int yresolution,int s_on, int r_on){
	x_resolution = xresolution;
	y_resolution = yresolution;
	op_filename = opfilename;
	shadow_on = s_on;
	if(shadow_on)
        std::cout<<"Shadow On"<<std::endl;
	reflection_on = r_on;
	if(reflection_on )
        std::cout<<"Reflection On"<<std::endl;

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
				float intensity = point_light->intensity.getValue();
                SbVec3f color = point_light->color.getValue();
				Light * new_light = new Light(location, intensity, color);
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

bool RayTracer::shadow_ray_intersection(SbVec3f *point_of_intersection, int light_source){

    float t_value;
    SbVec3f ray_direction;
    float epsilon = 0.01;
    ray_direction = lights.at(light_source).position - *point_of_intersection ;
    ray_direction.normalize();
    SbVec3f poi;
    poi = *point_of_intersection + (epsilon * ray_direction);
    for(int j=0; j< spheres.size(); j++){
        if(spheres.at(j).intersection(&poi, &ray_direction, &t_value))
            return true;

    }
    return false;
}
bool RayTracer::refract(SbVec3f *ray_direction, SbVec3f *normal_at_intersection, SbVec3f *T){
    float under_sqrt = 0;
    float refractive_index = RI;

    SbVec3f N, d, V;

    N = *normal_at_intersection;
    d = *ray_direction;
    V = -1 * d;
    //d = -1 *(*ray_direction);
    d.normalize();
    N.normalize();
    V.normalize();
    refractive_index = 1.0 / refractive_index;

    if (N.dot(d) >= 0 ){
        //std::cout<<"Going Out of medium : "<< N.dot(d)<<std::endl;
        N = -1 * N;
        N.normalize();
        refractive_index = 1/refractive_index;
    }
    else{
        //std::cout<<"Inside medium : "<< N.dot(d)<<std::endl;
    }

    under_sqrt = should_transmit(&d, &N, refractive_index);
    //std::cout<< under_sqrt<<std::endl;
    if(under_sqrt > 0){
        *T = (((refractive_index * N.dot(V)) - sqrt(under_sqrt))* N ) - (refractive_index * V);
        //*T =   ((refractive_index * (N.dot((-1 * d)) - sqrt(under_sqrt)))* (N)) - (refractive_index * (-1 * d));
        //std::cout<<"SQRT POS"<<std::endl;
        return true;
    }
   // std::cout<<"SQRT neg"<<std::endl;
    return false;
}

float RayTracer::should_transmit(SbVec3f *ray_direction, SbVec3f *normal_at_intersection, float refractive_index){
    float ret;
    SbVec3f V;
    SbVec3f N;
    float ri;

    V = -1 * *ray_direction;
    N = *normal_at_intersection;
    ri = refractive_index;
    //ret = 1- (refractive_index*refractive_index*(1-(normal_at_intersection->dot(-1 *(*ray_direction))*(normal_at_intersection->dot(-1 * (*ray_direction))))));
    ret = 1- ((ri*ri)*(1-( N.dot(V) * N.dot(V) )));
    if (ret < 0) ret = -1;
    return ret;
}


SbVec3f RayTracer::reflect(SbVec3f *normal_at_intersection, SbVec3f *ray_direction){
    SbVec3f  R;
    R = (-2 *(normal_at_intersection->dot(*ray_direction)* *normal_at_intersection)) + *ray_direction;
    R.normalize();
    return R;
}


bool RayTracer::shade(SbVec3f *ray_origin, SbVec3f *ray_direction, SbVec3f *retColor, int recursionDepth, int flag){
	float t_value, t_min = 999;
	SbVec3f normal_at_intersection;
	bool should_color = false;
    SbVec3f color;
    color[0] = 0.0;
    color[1] = 0.0;
    color[2] = 0.0;
    for(int k =0; k<spheres.size(); k++){
        Sphere temp = spheres.at(k);

        if(temp.intersection(ray_origin, ray_direction, &t_value))
        {
            if(t_value < t_min && t_value > 0 && t_value !=999) {
                t_min = t_value;
                SbVec3f V = -(*ray_direction); //view vector

                V.normalize();
                normal_at_intersection = temp.calculate_normal(ray_origin, ray_direction, t_value);
                normal_at_intersection.normalize(); // N vector at the point of intersection
                SbVec3f point_of_intersection = temp.point_of_intersection( ray_origin, ray_direction, t_value);

                for(int i = 0; i <3; i++) {// set the ambient color component
                        color[i] = (0.2 *  temp.material->ambientColor[0][i]);
                }
                // iterate through all the lights and add the diffuse and specular component
                for(int j = 0; j < lights.size(); j++){

                        bool shadowFlag = false;
                        if(shadow_on )
                            shadowFlag = shadow_ray_intersection(&point_of_intersection, j );
                            //shadowFlag = true;
                        if(!shadowFlag){

                            SbVec3f L = lights.at(j).position - point_of_intersection;
                            L.normalize();
                            SbVec3f R;
                            R = (2 * normal_at_intersection.dot(L) * normal_at_intersection) - L;
                            R.normalize();
                            //SbVec3f H = (V + L);//is using half way vector
                            //H.normalize();//is using half way vector
                            //float cos_theta = H.dot(normal_at_intersection); //is using half way vector

                            float NdotL = normal_at_intersection.dot(L);
                            float cos_theta = V.dot(R);

                            for(int i = 0; i <3; i++){
                                if(NdotL > 0)
                                    color[i] += ( NdotL * temp.material->diffuseColor[0][i] * lights.at(j).intensity * lights.at(j).color[i]);
                                if(cos_theta > 0)
                                    color[i] += ( pow(cos_theta, 20) * temp.material->specularColor[0][i]* lights.at(j).intensity * lights.at(j).color[i]);
                            }
                        }

                }
                SbVec3f refColor(0.0,0.0,0.0);
                SbVec3f refracColor(0.0,0.0,0.0);
                // if the current depth of recustion is less than the maximum depth,
                //reflect the ray and add the color returned dude to the result of reflection
                if(reflection_on && recursionDepth < 5){
                        if(temp.isShiny){
                            std::cout <<"shiny";
                            // compute replection of the ray, R1
                            SbVec3f R1;
                            //R1 = (-2 *(normal_at_intersection.dot(*ray_direction)* normal_at_intersection)) + *ray_direction;
                            //R1.normalize();
                            R1 = reflect(&normal_at_intersection, ray_direction);
                            SbVec3f poi;
                            poi = point_of_intersection + (0.01* R1);
                            shade(&poi, &R1, &refColor, recursionDepth+1);
                            color = color + (temp.shininess * refColor);
                        }
                        if(temp.isTransparent){
                            //std::cout<< "SPhere : " << k<<std::endl;
                            //std::cout << temp.transparency;
                            SbVec3f T;
                            if(refract(ray_direction, &normal_at_intersection, &T)){
                                //T = -1 * T;
                                T.normalize();
                                SbVec3f poi;
                                poi = point_of_intersection + (0.01* T);
                                shade(&poi, &T, &refracColor, recursionDepth+1,1);
                                //std::cout<<std::endl;
                                //print_vector(refracColor);
                                color = color + (temp.transparency * refracColor);
                                //print_vector(color);
                            }

                        }
                    }
                should_color = true;
            }

        }
    }
    *retColor = color;
    return should_color;
}

void RayTracer::trace_rays(){
	int i,j;
	SbVec3f pix_pos, d_vec;
	double a,b,c;
	float color_scale;
	SbVec3f color;
    std::vector<Pixel> image_row;
	std::cout << "Tracing rays"<<std::endl;
    color[0] = 0.0;
    color[1] = 0.0;
    color[2] = 0.0;

	for (i=0; i < y_resolution; i++){
        image_row.clear();
	    for (j=0; j < x_resolution; j++) {
            pix_pos = calculate_pixel_location(i,j);
            d_vec  = pix_pos - camera->position;
            d_vec.normalize();
            bool should_color = shade(&(camera->position), &d_vec, &color, 1);
            if(should_color)
            {
                image_row.push_back(Pixel(min(color[0]), min(color[1]), min(color[2])));
            }
            else
                image_row.push_back(Pixel(0,0,0));

        }
        image.push_back(image_row);
	}
	open_file();
	write_to_file(image);
	close_file();
	std::cout<<"Done Tracing"<<std::endl;
	return;
}

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
    val = fabs(val);
    // multiply the color component by white light and clamp it to 255
    return ( val*255 > 255 ? 255 : floor(val*255));

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
