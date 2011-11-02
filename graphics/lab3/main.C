#include "raytracer.h"

int main(int argc, char *argv[])
{
	using namespace std;
	string strOutFilename = "no";
	string strInputFilename = "no";
	int nXResolution = 0;
	int shadow_on = 0;
	int reflection_on = 0;
	int nYResolution = 0;
	if ( argc != 7){
		cout << "Usage Error:\nNeeds to be of the form\nrt <input.iv> <output.ppm> <xres> <yres> <shadow_on> <reflection_on>"<< endl;
		exit(0);
	}
	else{
		strInputFilename = argv[1];
		strOutFilename = argv[2];
		nXResolution = atoi(argv[3]);
		nYResolution = atoi(argv[4]);
		shadow_on = atoi(argv[5]);
		reflection_on = atoi(argv[6]);
	}
	RayTracer rt(strInputFilename, strOutFilename, nXResolution, nYResolution, shadow_on, reflection_on);
	rt.trace_rays();
	return 0;
}
