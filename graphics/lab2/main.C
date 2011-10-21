//#include "common.h"
//#include "sphere.h"
//#include "camera.h"
//#include "light.h"
#include "raytracer.h"


//using namespace std;
int main(int argc, char *argv[])
{
	using namespace std;
	string strOutFilename = "no";
	string strInputFilename = "no";
	int nXResolution = 0;
	int nYResolution = 0;
	if ( argc != 5){
		cout << "Usage Error:\nNeeds to be of the form\nrt <input.iv> <output.ppm> <xres> <yres>"<< endl;
		exit(0);
	}
	else{
		strInputFilename = argv[1];
		strOutFilename = argv[2];
		nXResolution = atoi(argv[3]);
		nYResolution = atoi(argv[4]);
	}
	RayTracer rt(strInputFilename, strOutFilename, nXResolution, nYResolution);
	rt.trace_rays();
	return 0;
}
