
#include <PNGViewer.h>
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

#include "lowPassGPU.h"

#ifdef __APPLE__
std::string pngPath = "../testdata/lenaGrayLarge.png";
#else
std::string pngPath = "..\\..\\testdata\\lenaGrayLarge.png";
#endif


int actualMain() {
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
	pngLoader->setData(reinterpret_cast<unsigned char*>(newData), pngLoader->getWidth(), pngLoader->getHeight());

	pngLoader->showWaitForEsc();
	/*
	lowPCPU->setData(data);

	lowPCPU->executeReference();
	newData = lowPCPU->getDataC();
	pngLoader->setData(reinterpret_cast<unsigned char*>(newData), pngLoader->getWidth(), pngLoader->getHeight());

	pngLoader->showWaitForEsc();
	*/
	delete pngLoader;
	return 0;
}

void openCLMain() {

	GPUImgProc::PNGViewer* pngLoader = new GPUImgProc::PNGViewer();

	pngLoader->setDataFromFile(pngPath);

	unsigned char* data;
	data = pngLoader->getDataSingleChannel();

	PerfUtility perf = PerfUtility();

	LowPassGPU locl;
	perf.measureFunction([&] {locl.initOpenCL(); }, "initOpenCL");

	perf.measureFunction([&]{ locl.uploadImage(data, pngLoader->getWidth(), pngLoader->getHeight()); }, "upload data");

	perf.measureFunction([&] {locl.execute();}, "execute");
	
	perf.measureFunction([&] {locl.downloadData(); }, "download data");

	perf.measureFunction([&] {data = locl.getDataC(); }, "getDataC");

	perf.printTotalTime();
	perf.printTimeSumBetween(1);


	pngLoader->setData(reinterpret_cast<unsigned char*>(data), pngLoader->getWidth(), pngLoader->getHeight());
	pngLoader->showWaitForEsc();




	/*
	OpenCLPlayground ocl;
	//ocl.listDevices();

	ocl.initData();

	std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

	ocl.initOpenCL();

	std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();

	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();

	std::cout << duration / 1000.0f << "ms" << std::endl;


	t1 = std::chrono::high_resolution_clock::now();

	ocl.addArrays();

	t2 = std::chrono::high_resolution_clock::now();

	duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();

	std::cout << duration / 1000.0f << "ms" << std::endl;

	ocl.initData();


	t1 = std::chrono::high_resolution_clock::now();

	ocl.testRef();

	t2 = std::chrono::high_resolution_clock::now();

	duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();

	std::cout << duration / 1000.0f << "ms" << std::endl;


	int i;
	std::cin >> i;
	*/
}

int main(int argc, char * argv[])
{
	openCLMain();
	actualMain();
	atexit(SDL_Quit);
	return 0;
}
