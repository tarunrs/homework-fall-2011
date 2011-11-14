#include "raytracer.h"

RayTracer::RayTracer(std::string ip_filename, std::string opfilename, int xresolution, int yresolution,int s_on, int r_on, int depth_of_field_on){
	x_resolution = xresolution;
	y_resolution = yresolution;
	op_filename = opfilename;
	shadow_on = s_on;
	if(shadow_on)
        std::cout<<"Shadow On"<<std::endl;
	reflection_on = r_on;
	if(reflection_on )
        std::cout<<"Reflection On"<<std::endl;
    focal_length = depth_of_field_on;

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
    float radius = 3.0;
	for(int  i= 0; i <scene->Lights.getLength(); i++){
		SoLight * light = (SoLight *) scene->Lights[i];
		SoType light_type = light->getTypeId();

		if(light->on.getValue() == true){
			if(light_type == SoPointLight::getClassTypeId()){
				SoPointLight * point_light = (SoPointLight*) light;
				SbVec3f location = point_light->location.getValue();
				float intensity = point_light->intensity.getValue();
                SbVec3f color = point_light->color.getValue();
				Light * new_light = new Light(location, intensity, color, radius);
				lights.push_back(*new_light);
			}
		}
	}
}
void RayTracer::calculate_image_dimentions(){
	float d = focal_length;
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


SbVec3f RayTracer::calculate_view_up(SbVec3f actual_ray_direction){
    SbVec3f rd;
    rd = actual_ray_direction;
    /*if(rd[0] == 0 && rd[1] ==0)
        { rd[0] =1; rd[2] = 0;}
    else if(rd[1] == 0 && rd[2] ==0)
        { rd[1] =1; rd[0] = 0;}
    else if(rd[2] == 0 && rd[0] ==0)
        { rd[2] =1; rd[1] = 0;}

    else*/ if(rd[0] > rd[1] && rd[0] > rd[2])
        rd[0] = 0;
    else if(rd[1] > rd[0] && rd[1] > rd[2])
        rd[1] = 0;
    else
        rd[2] = 0;

    return rd;
}

void RayTracer::calculate_coordinate_system(SbVec3f *u, SbVec3f *v, SbVec3f *n, SbVec3f actual_ray_direction){
    SbVec3f view_up;
    view_up = calculate_view_up(actual_ray_direction);
    //print_vector(view_up);
    view_up.normalize();
    //print_vector(view_up);
    *n = -1 * actual_ray_direction;
    n->normalize();
	*u = view_up.cross(*n);
    u->normalize();
	*v = n->cross(*u);
    v->normalize();

}

float RayTracer::soft_shadow_ray_intersection(SbVec3f *point_of_intersection, int light_source){

    SbVec3f actual_ray_direction, offset_ray_direction;
    SbVec3f u, v, n;
    int number_of_shadow_rays = NUMBER_OF_SHADOW_RAYS;
    float epsilon = 0.01;
    float retValue = 0;
    float R = 3;
    int count = 0;

    actual_ray_direction = lights.at(light_source).position - *point_of_intersection ;
    actual_ray_direction.normalize();
    SbVec3f poi;
    poi = *point_of_intersection + (epsilon * actual_ray_direction);
    calculate_coordinate_system(&u, &v, &n, actual_ray_direction);

    for(int i =0; i< number_of_shadow_rays; i++){

            float du, dv;
            float t;
            du = get_random_number();
            dv = get_random_number();
            if(i == 0)
            {
                du = 0.5;
                dv = 0.5;
            }
            offset_ray_direction = actual_ray_direction - (R/2 * u) - (R/2 * v) + (du * R * u) + (dv *R * v);
            offset_ray_direction.normalize();

        if(!shadow_ray_intersection(&poi, &offset_ray_direction, light_source)){
            if(lights.at(light_source).intersection(&poi, &offset_ray_direction)){
                float coss = fabs(actual_ray_direction.dot(offset_ray_direction)) -0.0001;
                if(coss > 1.0 || coss <0 )
                    std::cout<<coss;//<<std::endl;
                retValue += coss;//retValue+=1;
                count++;
            //std::cout<<"intersects";
            }
        }
    }
    if(count > 0 )
        retValue = retValue / count;//number_of_shadow_rays;
    else
        retValue =0;
    std::cout << "COunt: "<<count <<std::endl;
    //std::cout << retValue <<std::endl;
    return retValue;
}


bool RayTracer::shadow_ray_intersection(SbVec3f *point_of_intersection, SbVec3f *ray_direction,  int light_source){
    float t_value;
    for(int j=0; j< objects.size(); j++){
        //Cube tempCube;
        bool intersects = false;
        int shapetype = 0;
        shapetype = objects.at(j).shapeType ;
        if(shapetype == 1){
            Sphere tempSphere = objects.at(j);
            intersects = tempSphere.intersection(point_of_intersection, ray_direction, &t_value);
            //intersects = tempSphere.intersection(&poi, &ray_direction, &t_value);
        }
        else{
            //std::cout<<"cube";
            Cube tempCube = objects.at(j);
            intersects = tempCube.intersection(point_of_intersection, ray_direction, &t_value);
            //intersects = tempCube.intersection(&poi, &ray_direction, &t_value);
            //temp = (Cube)tempCube;
        }
        if(intersects)
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
	int closest_object = -1;
    SbVec3f color;
    color[0] = 0.0;
    color[1] = 0.0;
    color[2] = 0.0;
    Object temp;
    Sphere tempSphere;
    Cube tempCube;
    Object *tempPtr;
    int shapetype = 0;

    //Cone *tempCone = new Cone();

    for(int k =0; k<objects.size(); k++){

        bool intersects = false;
        shapetype = objects.at(k).shapeType ;
        if(shapetype == 1){
            //std::cout<<k<<"\n";
            tempSphere = objects.at(k);
            //std::cout<<k<<"\n";
            intersects = tempSphere.intersection(ray_origin, ray_direction, &t_value);
            //std::cout<<k<<"\n";
        }
        else{
            std::cout<<"cube";
            //std::cout<<"cube";
            tempCube = objects.at(k);
            intersects = tempCube.intersection(ray_origin, ray_direction, &t_value);
            //temp = (Cube)tempCube;
        }

        if(intersects)
        {
            if(t_value < t_min && t_value > 0 && t_value !=999) {
                t_min = t_value;
                closest_object = k;

            }
        }

    }// for loop ends here. should have the index of the closest object

    if(closest_object != -1){
                SbVec3f point_of_intersection ;
                temp = objects.at(closest_object);
                if(temp.shapeType == 1){
                    //temp = objects.at(closest_object);
                    tempSphere = objects.at(closest_object);
                    normal_at_intersection = tempSphere.calculate_normal(ray_origin, ray_direction, t_value);
                    normal_at_intersection.normalize(); // N vector at the point of intersection
                    point_of_intersection = tempSphere.point_of_intersection( ray_origin, ray_direction, t_value);
                    temp = tempSphere;
                    //tempPtr = &temp;
                }else{
                    tempCube = objects.at(closest_object);
                    normal_at_intersection = tempCube.calculate_normal(ray_origin, ray_direction, t_value);
                    normal_at_intersection.normalize(); // N vector at the point of intersection
                    point_of_intersection = tempCube.point_of_intersection( ray_origin, ray_direction, t_value);
                    temp = tempCube;
                    //tempPtr = &tempCube;
                }


                for(int i = 0; i <3; i++) {// set the ambient color component
                        color[i] = (0.2 *  temp.material->ambientColor[0][i] * (1 - temp.transparency ));
                }
                // iterate through all the lights and add the diffuse and specular component
                for(int j = 0; j < lights.size(); j++){

                        bool shadowFlag = false;
                        //float shadowLevel = 1;

                    if(shadow_on != 0)
                    {

                         //shadowLevel = soft_shadow_ray_intersection(&point_of_intersection, j );
                            SbVec3f actual_ray_direction, offset_ray_direction;
                            SbVec3f tempu, tempv, tempn;
                            int number_of_shadow_rays;
                            if(shadow_on == 1)
                                number_of_shadow_rays = 1;
                            else
                                number_of_shadow_rays = NUMBER_OF_SHADOW_RAYS;
                            float epsilon = 0.01;
                            float R = 0.1;
                            actual_ray_direction = lights.at(j).position - point_of_intersection ;
                            actual_ray_direction.normalize();
                            SbVec3f point = point_of_intersection + (epsilon * actual_ray_direction);

                            calculate_coordinate_system(&tempu, &tempv, &tempn, actual_ray_direction);

                            for(int ir =0; ir< number_of_shadow_rays; ir++){

                                    float du, dv;
                                    //float t;
                                    du = get_random_number();
                                    dv = get_random_number();
                                    du = R * (du - 0.5);
                                    dv = R * (dv - 0.5);
                                    if(shadow_on == 1) {du =0; dv =0;}
                                    //offset_ray_direction = actual_ray_direction - (R/2 * u) - (R/2 * v) + (du * R * u) + (dv *R * v);
                                    offset_ray_direction = actual_ray_direction + (du * tempu) + (dv * tempv);
                                    offset_ray_direction.normalize();
                                    SbVec3f poi;
                                    poi = point + (epsilon * offset_ray_direction);
                                    //offset_ray_direction = actual_ray_direction;
                                    if(!shadow_ray_intersection(&poi, &offset_ray_direction, j)){
                                        //normal_at_intersection = temp.calculate_normal(&poi, &offset_ray_direction, t_value);
                                        //normal_at_intersection.normalize();
                                        SbVec3f V = -1 * (*ray_direction); //view vector
                                        V.normalize();
                                        SbVec3f L = offset_ray_direction;
                                        L.normalize();
                                        SbVec3f R;
                                        R = (2 * normal_at_intersection.dot(L) * normal_at_intersection) - L;
                                        R.normalize();

                                        float NdotL = normal_at_intersection.dot(L);
                                        float cos_theta = V.dot(R);
                                        for(int i = 0; i <3; i++){
                                            {
                                                if(NdotL > 0)
                                                    color[i] += (( NdotL * temp.material->diffuseColor[0][i] * lights.at(j).intensity * lights.at(j).color[i]  * (1 - temp.transparency ))/ number_of_shadow_rays);
                                                if(cos_theta > 0)
                                                    color[i] += (( pow(cos_theta, 50) * temp.material->specularColor[0][i]* lights.at(j).intensity * lights.at(j).color[i]) / number_of_shadow_rays);
                                            }
                                        }

                                    }

                    }

                    }
                }
                SbVec3f refColor(0.0,0.0,0.0);
                SbVec3f refracColor(0.0,0.0,0.0);
                // if the current depth of recustion is less than the maximum depth,
                //reflect the ray and add the color returned dude to the result of reflection
                if(reflection_on && recursionDepth < 2){

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
                        else if(temp.isShiny){
                            // compute replection of the ray, R1
                            SbVec3f R1;
                            R1 = reflect(&normal_at_intersection, ray_direction);
                            SbVec3f poi;
                            poi = point_of_intersection + (0.01* R1);
                            shade(&poi, &R1, &refColor, recursionDepth+1);
                            color = color + (temp.shininess * refColor);
                        }
                    }
                should_color = true;
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

    for(int k =0; k< NUMBER_OF_SAMPLES; k++){
        du = get_random_number();
        dv = get_random_number();
        //pix_pos = calculate_pixel_location(i,j, du, dv);
        pix_pos = calculate_pixel_location(i,j, 0.5, 0.5);
        d_vec  = pix_pos - *position;
        d_vec.normalize();
//        std::cout<<"Distribute shade";
        should_color = shade(position, &d_vec, &tempColor, 1);
        *color = *color + tempColor;
    }
    *color = *color/NUMBER_OF_SAMPLES;
    return should_color;

}

bool RayTracer::depth_of_field(int i, int j, SbVec3f *position, SbVec3f *color){
    SbVec3f pix_pos, d_vec;
    SbVec3f tempColor;
    float du;
    float dv;
    bool should_color ;
    int number_of_jitter_positions =NUMBER_OF_CAMERAS;

    for(int k =0; k< number_of_jitter_positions ; k++){
        SbVec3f camera_position = *position;
        du = get_random_number();
        dv = get_random_number();
        //std::cout<<du<<dv;
        //pix_pos = calculate_pixel_location(i,j, du, dv);
        u.normalize();
        v.normalize();
        camera_position = camera_position  + (du * u) + (dv *v);
        //camera_position += (dv * v);
        pix_pos = calculate_pixel_location(i,j, 0.5, 0.5);
        d_vec  = pix_pos - (camera_position);
        d_vec.normalize();
//        std::cout<<"Distribute shade";
        should_color = shade(&camera_position, &d_vec, &tempColor, 1);
        *color = *color + tempColor;
    }
    *color = *color/number_of_jitter_positions ;
    return should_color;

}


void RayTracer::trace_rays(){
	int i,j;
    SbVec3f pix_pos, d_vec;
    SbVec3f tempColor;
    bool should_color ;

	double a,b,c;
	float color_scale;
	SbVec3f color;
    std::vector<Pixel> image_row;
    color.setValue(0.0,0.0,0.0);
	for (i=0; i < y_resolution; i++){
	    image_row.clear();
        for (j=0; j < x_resolution; j++) {
	        //bool should_color = distribute_shade(i, j, &(camera->position), &color);
	        //bool should_color = depth_of_field(i, j, &(camera->position), &color);
            pix_pos = calculate_pixel_location(i,j, 0.5, 0.5);
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
