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
	std::cout<<"Finished reading objects";
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

SbVec3f RayTracer::calculate_pixel_location(int i, int j, float du, float dv){
	SbVec3f pos;
	pos = image_L + (u * (i + du) * pixel_width) + ( v * (j + dv) * pixel_height);
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

                SoType shape_type = obj->shape->getTypeId();

                std::cout << "Object " << i << " is a "
                 << shape_type.getName().getString() << "." << std::endl;

                if (shape_type == SoSphere::getClassTypeId()) {
                    Sphere * temp = new Sphere(obj);
                    objects.push_back(*temp);
                };

                if (shape_type == SoCube::getClassTypeId()) {
                    Cube * temp = new Cube(obj);
                    objects.push_back(*temp);
                };
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
    for(int j=0; j< objects.size(); j++){
        if(objects.at(j).intersection(&poi, &ray_direction, &t_value))
            return true;

    }
    return false;
}

bool RayTracer::refract(SbVec3f *ray_direction, SbVec3f *normal_at_intersection, SbVec3f *T){
    float under_sqrt = 0;
    float refractive_index = 0;

    SbVec3f N, d, V;

    N = *normal_at_intersection;
    d = *ray_direction;
    V = -1 * d;
    d.normalize();
    N.normalize();
    V.normalize();
    refractive_index = 1.0 / RI;

    if (N.dot(d) >= 0 ){
        //std::cout<<"out";
        N = -1 * N;
        N.normalize();
        refractive_index = RI / 1.0 ;
    }

    under_sqrt = should_transmit(&d, &N, refractive_index);
    if(under_sqrt > 0){
        *T = (((refractive_index * N.dot(V)) - sqrt(under_sqrt))* N ) - (refractive_index * V);
        return true;
    }
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
	SbVec3f normal_at_intersection1;
	bool should_color = false;
    SbVec3f color;
    color[0] = 0.0;
    color[1] = 0.0;
    color[2] = 0.0;
    //Cone *tempCone = new Cone();
    for(int k =0; k<objects.size(); k++){
        //Object temp1 ;
        //temp1 = spheres.at(k);
        Sphere tempSphere;
        Cube tempCube;
        Object temp;
        bool intersects = false;
        int shapetype = 0;
        shapetype = objects.at(k).shapeType ;
        if(shapetype == 1){
            tempSphere = objects.at(k);
            intersects = tempSphere.intersection(ray_origin, ray_direction, &t_value);
        }
        else{
            //std::cout<<"cube";
            tempCube = objects.at(k);
            intersects = tempCube.intersection(ray_origin, ray_direction, &t_value);
            //temp = (Cube)tempCube;
        }

        if(intersects)
        {
            if(t_value < t_min && t_value > 0 && t_value !=999) {
                t_min = t_value;
                SbVec3f V = -(*ray_direction); //view vector
                V.normalize();
                SbVec3f point_of_intersection ;

                if(shapetype == 1){
                    normal_at_intersection = tempSphere.calculate_normal(ray_origin, ray_direction, t_value);
                    normal_at_intersection.normalize(); // N vector at the point of intersection
                    point_of_intersection = tempSphere.point_of_intersection( ray_origin, ray_direction, t_value);
                    temp = tempSphere;
                }else{
                    normal_at_intersection = tempCube.calculate_normal(ray_origin, ray_direction, t_value);
                    normal_at_intersection.normalize(); // N vector at the point of intersection
                    point_of_intersection = tempCube.point_of_intersection( ray_origin, ray_direction, t_value);
                    temp = tempCube;
                }


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
                            // compute replection of the ray, R1
                            SbVec3f R1;
                            R1 = reflect(&normal_at_intersection, ray_direction);
                            SbVec3f poi;
                            poi = point_of_intersection + (0.01* R1);
                            shade(&poi, &R1, &refColor, recursionDepth+1);
                            color = color + (temp.shininess * refColor);
                        }
                        if(temp.isTransparent){
                            SbVec3f T;
                            if(refract(ray_direction, &normal_at_intersection, &T)){
                                T.normalize();
                                SbVec3f poi;
                                poi = point_of_intersection + (0.01* T);
                                shade(&poi, &T, &refracColor, recursionDepth+1,1);
                                color = color + (temp.transparency * refracColor);
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

bool RayTracer::distribute_shade(int i, int j, SbVec3f *position, SbVec3f *color){
    //std::cout<<"intersecting cube";
    SbVec3f pix_pos, d_vec;
    SbVec3f tempColor;
    float du;
    float dv;
    bool should_color ;

    for(int k =0; k<2; k++){
        du = get_random_number();
        dv = get_random_number();
        pix_pos = calculate_pixel_location(i,j, 0.5, 0.5);
        d_vec  = pix_pos - *position;
        d_vec.normalize();
//        std::cout<<"Distribute shade";
        should_color = shade(position, &d_vec, &tempColor, 1);
        *color = *color + tempColor;
    }
    *color = *color/2;
    return should_color;

}


void RayTracer::trace_rays(){
	int i,j;

	double a,b,c;
	float color_scale;
	SbVec3f color;
    std::vector<Pixel> image_row;
    color.setValue(0.0,0.0,0.0);
	for (i=0; i < y_resolution; i++){
	    image_row.clear();
        for (j=0; j < x_resolution; j++) {
	        bool should_color = distribute_shade(i, j, &(camera->position), &color);
            //pix_pos = calculate_pixel_location(i,j);
            //d_vec  = pix_pos - camera->position;
            //d_vec.normalize();
            //bool should_color = shade(&(camera->position), &d_vec, &color, 1);
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
