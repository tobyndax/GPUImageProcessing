#pragma once

#ifdef __APPLE__
#include "../thirdparty/OpenCL1.2/cl.hpp"
#else
//#include "CL/CL.h" 
#include "../thirdparty/OpenCL1.2/cl.hpp"
#endif

class LowPassOpenCL {

public:

	LowPassOpenCL(cl_device_type);
	~LowPassOpenCL();

	void initOpenCL();
	void uploadImage(unsigned char * imPtr, int width, int height);
	void execute();
	void downloadData();
	void swapBuffers();
	void transpose();

	unsigned char * getDataC();
	void executeTransposeOnce();
	void executeLowPassGPUOnce();


private:

	void setData(unsigned char * aData);

	//CPU variables
	float* input = nullptr;
	float* output = nullptr;
	unsigned char * outputC = nullptr;

	int width = 0; 
	int height = 0;

	//OpenCL members
	cl_device_type clDevType = NULL; 
	cl_device_id device_id = NULL;
	cl_context context = NULL;
	cl_command_queue command_queue = NULL;
	cl_kernel transposeKernel = NULL;
	cl_kernel lowPassCPUKernel = NULL;
	cl_kernel lowPassGPUKernel = NULL;
	cl_mem input_mem_obj = NULL;
	cl_mem output_mem_obj = NULL;
};