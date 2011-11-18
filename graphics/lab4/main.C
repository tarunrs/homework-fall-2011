#include "raytracer.h"

int main(int argc, char *argv[])
{
	using namespace std;
	string strOutFilename = "no";
	string strInputFilename = "no";
	int nXResolution = 0;
	int refraction_on = 0;
	int shadow_on = 0;
	int reflection_on = 0;
	int nYResolution = 0;
	int depth_of_field_on = 0;
	int super_sampling =0;
	if ( argc != 10){
		cout << "Usage Error:\nNeeds to be of the form\nrt <input.iv> <output.ppm> <xres> <yres> <pixel-supersampling> <shadow_on> <reflection_on> <refraction_on> <depth_of_field>"<< endl;
		exit(0);
	}
	else{
		strInputFilename = argv[1];
		strOutFilename = argv[2];
		nXResolution = atoi(argv[3]);
		nYResolution = atoi(argv[4]);
        super_sampling = atoi(argv[5]);
		shadow_on = atoi(argv[6]);
		reflection_on = atoi(argv[7]);
		refraction_on = atoi(argv[8]);
		depth_of_field_on = atoi(argv[9]);

	}
	RayTracer rt(strInputFilename, strOutFilename, nXResolution, nYResolution, shadow_on, reflection_on, refraction_on, depth_of_field_on, super_sampling);
	rt.trace_rays();
	return 0;
}
