
#include <PNGViewer.h>
//#include <lowPassAMP.h>
#include <sdl.h> // Need this for SDL_Quit at exit
#include <lowPassCPU.h>
#include <iostream>
#include <openCLPlayground.h>
#include <chrono>
#include "PerfUtility.h"
#ifdef __APPLE__
#include "../thirdparty/OpenCL1.2/cl.hpp"
#else
//#include "../thirdparty/OpenCL1.2/cl.hpp"
#endif

#include <utility>
#include <fstream>
#include <string>

#include "lowPassOpenCL.h"

#ifdef __APPLE__
std::string pngPath = "../testdata/lenaGrayLarge.png";
#else
std::string pngPath = "..\\..\\testdata\\lenaGrayLarge.png";
#endif

int refMain() {

	GPUImgProc::PNGViewer* pngLoader = new GPUImgProc::PNGViewer();

	pngLoader->setDataFromFile(pngPath);

	unsigned char* data;
	data = pngLoader->getDataSingleChannel();
	unsigned char* newData;

	PerfUtility perf = PerfUtility();
	GPUImgProc::LowPassCPU *lowPCPU;
	
	perf.measureFunction([&] {lowPCPU = new GPUImgProc::LowPassCPU(data, pngLoader->getWidth(), pngLoader->getHeight()); }, "init LowPassCPU");

	perf.measureFunction([&] {lowPCPU->execute(); }, "execute");

	perf.measureFunction([&] {newData = lowPCPU->getDataC(); }, "getDataC");

	perf.printTotalTime();
	std::cout << "########################################" << std::endl << std::endl;


	pngLoader->setData(reinterpret_cast<unsigned char*>(newData), pngLoader->getWidth(), pngLoader->getHeight());

	pngLoader->showWaitForEsc();
	
	delete pngLoader;
	return 0;
}

void openCLMain() {

	GPUImgProc::PNGViewer* pngLoader = new GPUImgProc::PNGViewer();

	pngLoader->setDataFromFile(pngPath);

	unsigned char* data;
	data = pngLoader->getDataSingleChannel();

	PerfUtility perf = PerfUtility();

	LowPassOpenCL* locl = new LowPassOpenCL(CL_DEVICE_TYPE_CPU);
	perf.measureFunction([&] {locl->initOpenCL(); }, "initOpenCL CPU Version");

	perf.measureFunction([&]{ locl->uploadImage(data, pngLoader->getWidth(), pngLoader->getHeight()); }, "upload data");

	perf.measureFunction([&] {locl->execute();}, "execute");
	
	perf.measureFunction([&] {locl->downloadData(); }, "download data");

	perf.measureFunction([&] {data = locl->getDataC(); }, "getDataC");

	perf.printTotalTime();
	perf.printTimeSumBetween(1);
	std::cout << "########################################" << std::endl << std::endl;

	pngLoader->setData(reinterpret_cast<unsigned char*>(data), pngLoader->getWidth(), pngLoader->getHeight());
	pngLoader->showWaitForEsc();

	perf.reset();
	pngLoader->setDataFromFile(pngPath);
	data = pngLoader->getDataSingleChannel();
	
	locl = new LowPassOpenCL(CL_DEVICE_TYPE_GPU);
	perf.measureFunction([&] {locl->initOpenCL(); }, "initOpenCL GPU Version");

	perf.measureFunction([&] { locl->uploadImage(data, pngLoader->getWidth(), pngLoader->getHeight()); }, "upload data");

	perf.measureFunction([&] {locl->execute(); }, "execute");

	perf.measureFunction([&] {locl->downloadData(); }, "download data");

	perf.measureFunction([&] {data = locl->getDataC(); }, "getDataC");

	perf.printTotalTime();
	perf.printTimeSumBetween(1);

	perf.measureFunction([&] {locl->executeLowPassGPUOnce(); }, "One Lowpass only");
	perf.measureFunction([&] {locl->executeTransposeOnce(); }, "One transpose only");

	std::cout << "########################################" << std::endl << std::endl;
	
	pngLoader->setData(reinterpret_cast<unsigned char*>(data), pngLoader->getWidth(), pngLoader->getHeight());
	pngLoader->showWaitForEsc();

}
//
//void ampMain() {
//	GPUImgProc::PNGViewer* pngLoader = new GPUImgProc::PNGViewer();
//
//	pngLoader->setDataFromFile(pngPath);
//
//	unsigned char* data;
//	data = pngLoader->getDataSingleChannel();
//
//	PerfUtility perf = PerfUtility();
//
//	LowPassAMP* lamp= new LowPassAMP();
//	perf.measureFunction([&] {lamp->init(); }, "init amp");
//	perf.measureFunction([&] { lamp->uploadData(data, pngLoader->getWidth(), pngLoader->getHeight()); }, "upload data");
//
//
//}


int main(int argc, char * argv[])
{
	//ampMain();
	openCLMain();
	refMain();
	atexit(SDL_Quit);
	return 0;
}
