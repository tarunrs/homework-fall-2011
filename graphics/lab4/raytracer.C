#include "raytracer.h"

RayTracer::RayTracer(std::string ip_filename, std::string opfilename, std::string shapefilename, int xresolution, int yresolution,int s_on, int r_on, int refract_on, int dof_on, int super_sampling){
	x_resolution = xresolution;
	y_resolution = yresolution;
	op_filename = opfilename;
	shape_filename = shapefilename;
	shadow_on = s_on;
	if(shadow_on)
        std::cout<<"Shadow On"<<std::endl;
	reflection_on = r_on;
	if(reflection_on )
        std::cout<<"Reflection On"<<std::endl;
    refraction_on = refract_on;
    if(refraction_on )
        std::cout<<"Refraction On"<<std::endl;
    super_sampling_on = super_sampling;
    if(super_sampling_on)
        std::cout<<"Supersampling On"<<std::endl;
    if(dof_on == 0 ){
        depth_of_field_on = 0;
        focal_length = 10;
    }
    else{
        focal_length = dof_on;
        depth_of_field_on = 1;
    }
    perlin_init();
	read_open_inventor_scene(ip_filename);

}

void RayTracer::read_open_inventor_scene(std::string iv_file){
	SoDB::init();
	scene = new OSUInventorScene((char *)iv_file.c_str());
	read_shapes();
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

                if (shapes.at(i) == 3) {
                    Cone * temp = new Cone(obj);
                    temp->texture = textures.at(i);
                    objects.push_back(*temp);
                }
                else  if (shapes.at(i) == 4) {
                    std::cout<<"Recognised Cylinder"<<std::endl;
                    Cylinder * temp = new Cylinder(obj);
                    temp->texture = textures.at(i);
                    objects.push_back(*temp);
                }
                else if (shape_type == SoSphere::getClassTypeId() || shapes.at(i) == 1) {
                    Sphere * temp = new Sphere(obj);
                    temp->texture = textures.at(i);
                    objects.push_back(*temp);
                }
                else if (shape_type == SoCube::getClassTypeId()  || shapes.at(i) == 2) {
                    Cube * temp = new Cube(obj);
                    temp->texture = textures.at(i);
                    objects.push_back(*temp);
                }
        }
}

void RayTracer::read_shapes(){
    std::cout<<"Reading shapes";
    std::string filename;
    filename = shape_filename;
    std::ifstream ifile;
    ifile.open(filename.c_str());
    std::string element;
    int i=0, j=0;
    while(!ifile.eof()){
        ifile >> element;
        if(element == "sphere")
            shapes.push_back(1);
        else if(element == "cube")
            shapes.push_back(2);
        else if(element == "cone")
            shapes.push_back(3);
        else if(element == "cylinder")
            shapes.push_back(4);
        else
            textures.push_back(atoi(element.c_str()));
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
            if(tempSphere.transparency > 0 && refraction_on) continue;
            intersects = tempSphere.intersection(point_of_intersection, ray_direction, &t_value);
            //intersects = tempSphere.intersection(&poi, &ray_direction, &t_value);
        }
        else if (shapetype ==2){
            //std::cout<<"cube";

            Cube tempCube = objects.at(j);
            if(tempCube.transparency > 0 && refraction_on) continue;
            intersects = tempCube.intersection(point_of_intersection, ray_direction, &t_value);
            //temp = (Cube)tempCube;
        }else if (shapetype == 3){
            Cone tempCone = objects.at(j);
            if(tempCone.transparency > 0 && refraction_on)  continue;
            intersects = tempCone.intersection(point_of_intersection, ray_direction, &t_value);

        }
        else{
            //std::cout<<"cube";
            Cube tempCylinder = objects.at(j);
            if(tempCylinder.transparency > 0 && refraction_on) continue;
            intersects = tempCylinder.intersection(point_of_intersection, ray_direction, &t_value);
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
        float tempTerm;
        tempTerm = ((refractive_index * N.dot(V)) - sqrt(under_sqrt));
        *T = (tempTerm * N ) - (refractive_index * V);
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
	float epsilon = 0.01;
	SbVec3f normal_at_intersection;
	SbVec3f normal_at_intersection1, actual_ray_direction ;
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
        Cone tempCone;
        Cylinder tempCylinder;
        Object temp;
        bool intersects = false;
        int shapetype = 0;
        shapetype = objects.at(k).shapeType ;
        //std::cout <<"Shapetype:"<<shapetype;
        if(shapetype == 1){
            tempSphere = objects.at(k);
            intersects = tempSphere.intersection(ray_origin, ray_direction, &t_value);
        }
        else if (shapetype ==2){
            //std::cout<<"cube";
            tempCube = objects.at(k);
            intersects = tempCube.intersection(ray_origin, ray_direction, &t_value);
            //temp = (Cube)tempCube;
        }else if (shapetype == 3){
            tempCone = objects.at(k);
            intersects = tempCone.intersection(ray_origin, ray_direction, &t_value);

        }else{
          //  std::cout<<"Here";
            tempCylinder = objects.at(k);
            intersects = tempCylinder.intersection(ray_origin, ray_direction, &t_value);

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
                }else if(shapetype == 2){
                    normal_at_intersection = tempCube.calculate_normal(ray_origin, ray_direction, t_value);
                    normal_at_intersection.normalize(); // N vector at the point of intersection
                    point_of_intersection = tempCube.point_of_intersection( ray_origin, ray_direction, t_value);
                    temp = tempCube;
                }else if(shapetype ==3){
                    normal_at_intersection = tempCone.calculate_normal(ray_origin, ray_direction, t_value);
                    normal_at_intersection.normalize(); // N vector at the point of intersection
                    point_of_intersection = tempCone.point_of_intersection( ray_origin, ray_direction, t_value);
                    temp = tempCone;

                }
                else{
                    normal_at_intersection = tempCylinder.calculate_normal(ray_origin, ray_direction, t_value);
                    normal_at_intersection.normalize(); // N vector at the point of intersection
                    point_of_intersection = tempCylinder.point_of_intersection( ray_origin, ray_direction, t_value);
                    temp = tempCylinder;

                }


                for(int i = 0; i <3; i++) {// set the ambient color component
                        color[i] = (0.2 *  temp.material->ambientColor[0][i] * (1 - temp.transparency ));
                }
                //*retColor = color; return true;//ntc
                // iterate through all the lights and add the diffuse and specular component
                for(int j = 0; j < lights.size(); j++){
                        SbVec3f poi;

                        actual_ray_direction = lights.at(j).position - point_of_intersection ;
                        actual_ray_direction.normalize();
                        poi = point_of_intersection + (epsilon * actual_ray_direction);
                        bool shadowFlag = false;
                        if(shadow_on == 0 || shadow_on == 1)
                        {
                            if(shadow_on == 1)
                                shadowFlag = shadow_ray_intersection(&poi, &actual_ray_direction , j );
                            //shadowFlag = true;
                            if(!shadowFlag)
                            {
                                SbVec3f L = lights.at(j).position - point_of_intersection;
                                L.normalize();
                                SbVec3f R;
                                R = (2 * normal_at_intersection.dot(L) * normal_at_intersection) - L;
                                R.normalize();

                                float NdotL = normal_at_intersection.dot(L);
                                float cos_theta = V.dot(R);
                                SbVec3f tempTexture = calculate_texture(point_of_intersection, &temp);
                                //print_vector(tempTexture);
                                for(int i = 0; i <3; i++){
                                    if(NdotL > 0)
                                        color[i] += (( NdotL * temp.material->diffuseColor[0][i] * lights.at(j).intensity * lights.at(j).color[i]  * (1 - temp.transparency ))) * tempTexture[i] ;
                                    if(cos_theta > 0)
                                        color[i] += (( pow(cos_theta, 50) * temp.material->specularColor[0][i]* lights.at(j).intensity * lights.at(j).color[i]) );
                                }
                            }
                        }
                    else
                    { // soft shadows
                            {

                         //shadowLevel = soft_shadow_ray_intersection(&point_of_intersection, j );
                            SbVec3f actual_ray_direction, offset_ray_direction;
                            SbVec3f tempu, tempv, tempn;
                            int number_of_shadow_rays;
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
                                    offset_ray_direction = actual_ray_direction + (du * tempu) + (dv * tempv);
                                    offset_ray_direction.normalize();

                                    //offset_ray_direction = actual_ray_direction - (R/2 * u) - (R/2 * v) + (du * R * u) + (dv *R * v);
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
                                        //if(temp.transparency > 0) std::cout<<"trnas";
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
                }
                SbVec3f refColor(0.0,0.0,0.0);
                SbVec3f refracColor(0.0,0.0,0.0);
                // if the current depth of recustion is less than the maximum depth,
                //reflect the ray and add the color returned dude to the result of reflection
                //std::cout<<"here";
                if(refraction_on && recursionDepth < 2){
                    if(temp.isTransparent){
                        SbVec3f T;
                        if(refract(ray_direction, &normal_at_intersection, &T)){
                            T.normalize();
                            SbVec3f poi;
                            poi = point_of_intersection + (epsilon * T);
                            shade(&poi, &T, &refracColor, recursionDepth+1);
                            color = color + (temp.transparency * refracColor);
                        }

                    }
                }
                if(reflection_on && recursionDepth < 2){

                    if(temp.isShiny){//} && !temp.isTransparent){
                        // compute replection of the ray, R1
                        SbVec3f R1;
                        R1 = reflect(&normal_at_intersection, ray_direction);
                        SbVec3f poi;
                        poi = point_of_intersection + (epsilon * R1);
                        shade(&poi, &R1, &refColor, recursionDepth+1);
                        color = color + ((1 - temp.transparency) * temp.shininess * refColor);

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
    SbVec3f pix_pos, d_vec;
    SbVec3f tempColor;
    float du;
    float dv;
    bool should_color ;
    int number_of_samples = 0;
    if(super_sampling_on == 0) {

        //std::cout<<"No Super sampling";
            //super sampling off
            pix_pos = calculate_pixel_location(i,j, 0.5, 0.5);
            d_vec  = pix_pos - *position;
            d_vec.normalize();
            should_color = shade(position, &d_vec, color, 1);
    }
    else{
        //super sampling on
        number_of_samples = NUMBER_OF_SAMPLES;
        for(int k =0; k< number_of_samples ; ++k){
            du = get_random_number();
            dv = get_random_number();
            pix_pos = calculate_pixel_location(i,j, du, dv);
            d_vec  = pix_pos - *position;
            d_vec.normalize();
            should_color = shade(position, &d_vec, &tempColor, 1);
            *color = *color + tempColor;
        }
        *color = *color/number_of_samples ;
    }
    return should_color;
}

bool RayTracer::depth_of_field(int i, int j, SbVec3f *position, SbVec3f *color){
    SbVec3f tempColor;
    float R = DISK_SIZE;
    float du;
    float dv;
    bool should_color ;
    int number_of_jitter_positions = NUMBER_OF_CAMERAS;

    if(depth_of_field_on == 0) {
            should_color = distribute_shade(i,j, position, color);
    }
    else{
        for(int k =0; k< number_of_jitter_positions ; k++){
            SbVec3f camera_position = *position;
            du = get_random_number();
            dv = get_random_number();
            camera_position = camera_position  + (du * R * pixel_width * this->u) + (dv * pixel_height * R * this->v);
            tempColor.setValue(0.0,0.0,0.0);
            should_color = distribute_shade(i, j, &camera_position,&tempColor);
            *color = *color + tempColor;
        }
        *color = *color/number_of_jitter_positions ;

    }
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
    std::cout<<"Complete ";
	for (i=0; i < y_resolution; i++){
	    if (i%20 == 0)
            std::cout<< i << "/"<<y_resolution<<" Complete"<<std::endl;
	    image_row.clear();
        for (j=0; j < x_resolution; j++) {
	        //bool should_color = distribute_shade(i, j, &(camera->position), &color);
	        bool should_color = depth_of_field(i, j, &(camera->position), &color);

	        //if()
            //pix_pos = calculate_pixel_location(i,j, 0.5, 0.5);
            //d_vec  = pix_pos - camera->position;
            //d_vec.normalize();
            //bool should_color = shade(&(camera->position), &d_vec, &color, 1);
            if(should_color)
            {
                image_row.push_back(Pixel(min(color[0]), min(color[1]), min(color[2])));
            }
            else
                image_row.push_back(Pixel(5,5,5));

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

SbVec3f RayTracer::calculate_texture(SbVec3f poi, Object* obj){
    SbVec3f color;
    float width = 0.1;
    float pie = 3.1432;
    color.setValue(1.0,1.0,1.0);
    if(obj->texture == 0)
        return color;
    SbVec3f poi_in_object_space;
    poi_in_object_space = obj->multiply_with_inverse(poi);

    if(obj->texture == TEXTURE_SMOOTH_COLORS){
        color[0] = 1 - fabs((2 * fract(poi_in_object_space[0]) -1));
        color[1] = 1 - fabs((2 * fract(poi_in_object_space[1]) -1));
        color[2] = 1 - fabs((2 * fract(poi_in_object_space[2]) -1));
    }
    else if(obj->texture == TEXTURE_STRIPS)
    {
        float sine = 0;
        sine = sin(pie* poi_in_object_space[2] / width);
        if(sine > 0)
            color.setValue(0.8,0.8,0.8);
    }
    else if(obj->texture == TEXTURE_CONCENTRIC_CIRCLES){
        float r;
        r = sqrtf(pow(poi_in_object_space[0],2) + pow(poi_in_object_space[1],2));
        if(int(r/width) % 2 == 0)
            color.setValue(0.0,0.0,0.0);
    }
    else if(obj->texture == TEXTURE_CHECKERBOARD){
        float r;
        float width = 0.2;
        r = ((int) (poi[0]/width) +
            (int) (poi[1]/width) +
            (int) (poi[2]/width) )
                % 2;
        if(r== 0)
            color.setValue(0.5,0.5,0.5);
    }else if(obj->texture == TEXTURE_WOOD_GRAIN){
            float retNoise1;
            retNoise1 = noise(poi[0],poi[1],poi[2]);
            color.setValue(retNoise1, retNoise1, retNoise1);

    }
    else if(obj->texture == TEXTURE_RANDOM_SURFACE){
            float retNoise1, retNoise2, retNoise3;
            retNoise1 = noise(poi[0],poi[1],poi[2]);
            retNoise2 = noise(poi[1],poi[2],poi[0]);
            retNoise3 = noise(poi[2],poi[0],poi[1]);

            color.setValue(retNoise1, retNoise2, retNoise3);
    }

    else if(obj->texture == TEXTURE_CUSTOM1){
        int r;
        float width = 0.4;
        r = abs(((int) (poi_in_object_space[0]/width) +
            (int) (poi_in_object_space[1]/width) +
            (int) (poi_in_object_space[2]/width) )
                % 10);
                std::cout<<r<<std::endl;
                float val = (float) r/ 10.0;
        color.setValue(val,val,val);
    }
    //color = poi_in_object_space;
    return color;

}

float RayTracer::fract(float val){
    return (val - floorf(val));
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
