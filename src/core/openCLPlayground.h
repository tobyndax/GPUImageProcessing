#pragma once
#include <iostream>

#ifdef __APPLE__
#include "../thirdparty/OpenCL1.2/cl.hpp"
#else
#include "CL/CL.h" 
//#include "../thirdparty/OpenCL1.2/cl.hpp"
#endif
class OpenCLPlayground {
public:
	OpenCLPlayground();
	~OpenCLPlayground();

	void test();


	void listDevices();

	void addArrays();
	void testRef();

	void initOpenCL();
	void initData();


private:
	float * A = nullptr;
	float * B = nullptr;
	float * C = nullptr;
	const int NUM_ELEMENTS = 1024*1024;
	cl_device_id device_id = NULL;
	cl_context context = NULL;
	cl_command_queue command_queue = NULL;
	cl_kernel kernel = NULL;
	cl_mem a_mem_obj = NULL;
	cl_mem b_mem_obj = NULL;
	cl_mem c_mem_obj = NULL;
};
