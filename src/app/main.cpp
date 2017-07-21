#include <PNGViewer.h>
#include <sdl.h> // Need this for SDL_Quit at exit
#include <lowPassCPU.h>
#include <iostream>
#include <openCLPlayground.h>

#ifdef __APPLE__
#include "../thirdparty/OpenCL1.2/cl.hpp"
#else
#include "CL/cl.hpp"
#endif

#include <utility>
#include <fstream>
#include <string>

int actualMain() {
	GPUImgProc::PNGViewer* pngLoader = new GPUImgProc::PNGViewer();
	#ifdef __APPLE__
	std::string pngPath = "../testdata/lenaGrayLarge.png";
	#else
	std::string pngPath = "..\\..\\testdata\\lenaGrayLarge.png";
	#endif
	pngLoader->setDataFromFile(pngPath);

	unsigned char* data;
	data = pngLoader->getDataSingleChannel();

	GPUImgProc::LowPassCPU *lowPCPU = new GPUImgProc::LowPassCPU(data, pngLoader->getWidth(), pngLoader->getHeight());

	lowPCPU->execute();
	unsigned char* newData = lowPCPU->getDataC();
	pngLoader->setData(reinterpret_cast<unsigned char*>(newData), pngLoader->getWidth(), pngLoader->getHeight());

	pngLoader->showWaitForEsc();

	lowPCPU->setData(data);

	lowPCPU->executeReference();
	newData = lowPCPU->getDataC();
	pngLoader->setData(reinterpret_cast<unsigned char*>(newData), pngLoader->getWidth(), pngLoader->getHeight());

	pngLoader->showWaitForEsc();

	delete pngLoader;
	return 0;
}

using namespace cl;
void openCLMain(){

	OpenCLPlayground ocl;
	ocl.listDevices();

/*
	//get all platforms (drivers)
	std::vector<Platform> platforms;
	Platform::get(&platforms);
	// Select the default platform and create a context using this platform and the GPU
	cl_context_properties cps[3] = {
		CL_CONTEXT_PLATFORM,
		(cl_context_properties)(platforms[0])(),
		0
	};
	Context context( CL_DEVICE_TYPE_CPU, cps);
	//Context context( CL_DEVICE_TYPE_GPU, cps);

	// Get a list of devices on this platform
	std::vector<Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();
	std::cout << devices.size() << std::endl;
*/
}

int main(int argc, char * argv[])
{

	openCLMain();

	atexit(SDL_Quit);
	return 0;
}
