
#include "openCLPlayground.h"
#include <iostream>
#include <algorithm>
#include <cassert>

#include <cmath>

#define MAX_SOURCE_SIZE (0x100000)


OpenCLPlayground::OpenCLPlayground() {

}

OpenCLPlayground::~OpenCLPlayground() {

}


void OpenCLPlayground::initOpenCL() {

	cl_platform_id platform_id = NULL;
	cl_uint ret_num_devices;
	cl_uint ret_num_platforms;

	cl_int ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);

	ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_CPU, 1, &device_id, &ret_num_devices);

	// Create an OpenCL context (which device?) 
	context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);

	// Create a command queue (why?)
	command_queue = clCreateCommandQueue(context, device_id, 0, &ret);

	FILE *fp;
	char *source_str;
	size_t source_size;

	fp = fopen("vectorAdd.cl", "r");
	if (!fp) {
		fprintf(stderr, "Failed to load kernel.\n");
		exit(1);
	}
	source_str = (char*)malloc(MAX_SOURCE_SIZE);
	source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
	fclose(fp);


	// Create a program from the kernel source
	cl_program program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, &ret);
	assert(ret == 0);


	// Build the program
	ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
	assert(ret == 0);
	// Create the OpenCL kernel
	kernel = clCreateKernel(program, "vector_add", &ret);
	assert(ret ==  0);

	// Create memory buffers on the device for each input array
	a_mem_obj = clCreateBuffer(context, CL_MEM_USE_HOST_PTR,
		NUM_ELEMENTS * sizeof(float), A, &ret);
	assert(ret == 0);

	b_mem_obj = clCreateBuffer(context, CL_MEM_USE_HOST_PTR,
		NUM_ELEMENTS * sizeof(float), B, &ret);
	assert(ret == 0);

	//output buffer
	c_mem_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
		NUM_ELEMENTS * sizeof(float), NULL, &ret);

	assert(ret == 0);
}
void OpenCLPlayground::initData() {
	if (A) delete A;
	if (B) delete B;
	if (C) delete C;
	A = new float[NUM_ELEMENTS];
	B = new float[NUM_ELEMENTS];
	C = new float[NUM_ELEMENTS];
	int i = 0;
	std::generate_n(A, NUM_ELEMENTS, [&i]() {return (float)i++; });
	std::generate_n(B, NUM_ELEMENTS, [&i]() {return (float)i--; });
	std::generate_n(C, NUM_ELEMENTS, []() {return 0; });
}

void OpenCLPlayground::addArrays()
{
	if (device_id == nullptr) {
		return; //this is an error.
	}

	cl_int ret;
	
	//Copy data to input buffers (why blocking writes?)
	//ret = clEnqueueWriteBuffer(command_queue, a_mem_obj, CL_TRUE, 0, NUM_ELEMENTS * sizeof(float), A, 0, NULL, NULL);
	//ret = clEnqueueWriteBuffer(command_queue, b_mem_obj, CL_TRUE, 0, NUM_ELEMENTS * sizeof(float), B, 0, NULL, NULL);

	// Set the arguments of the kernel
	ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&a_mem_obj);
	assert(ret == 0);

	ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&b_mem_obj);
	assert(ret == 0);

	ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&c_mem_obj);
	assert(ret == 0);

	// Execute the OpenCL kernel on the list
	size_t global_item_size = NUM_ELEMENTS/4; // Process the entire lists (divide by 16, due to float16 operations)
	size_t local_item_size = 32; // Divide work items into groups of 64
	
	ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL,
		&global_item_size, &local_item_size, 0, NULL, NULL);
	assert(ret == 0);
	// Read the memory buffer C on the device to the local variable C
	ret = clEnqueueReadBuffer(command_queue, c_mem_obj, CL_TRUE, 0,
		NUM_ELEMENTS * sizeof(float), C, 0, NULL, NULL);
	assert(ret == 0);


	assert(sqrt(pow(A[1000],2) + pow(B[1000],2)) == C[1000]);
	//assert(A[NUM_ELEMENTS-1] + B[NUM_ELEMENTS-1] == C[NUM_ELEMENTS-1]);
	int i = 0;
	
}

void OpenCLPlayground::testRef() {

	float * __restrict aPtr = A;
	float * __restrict bPtr = B;
	float * __restrict cPtr = C;

	for (int i = 0; i < NUM_ELEMENTS; i++) {
		*cPtr = sqrt(pow(*aPtr,2) + pow(*bPtr,2));
		aPtr++;
		bPtr++;
		cPtr++;
	}

	int i = 0;
}
/*
void OpenCLPlayground::listDevices() {
	std::vector<cl::Platform> platforms;

	cl::Platform::get(&platforms);

	int platform_id = 0;
	int device_id = 0;

	std::cout << "Number of Platforms: " << platforms.size() << std::endl;

	for (std::vector<cl::Platform>::iterator it = platforms.begin(); it != platforms.end(); ++it) {
		cl::Platform platform(*it);

		std::cout << "Platform ID: " << platform_id++ << std::endl;
		std::cout << "Platform Name: " << platform.getInfo<CL_PLATFORM_NAME>() << std::endl;
		std::cout << "Platform Vendor: " << platform.getInfo<CL_PLATFORM_VENDOR>() << std::endl;
		std::vector<cl::Device> devices;

		platform.getDevices(CL_DEVICE_TYPE_GPU | CL_DEVICE_TYPE_CPU, &devices);

		for (std::vector<cl::Device>::iterator it2 = devices.begin(); it2 != devices.end(); ++it2) {
			cl::Device device(*it2);

			std::cout << "\tDevice " << device_id++ << ": " << std::endl;
			std::cout << "\t\tDevice Name: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
			std::cout << "\t\tDevice Type: " << device.getInfo<CL_DEVICE_TYPE>();
			std::cout << " (GPU: " << CL_DEVICE_TYPE_GPU << ", CPU: " << CL_DEVICE_TYPE_CPU << ")" << std::endl;
			std::cout << "\t\tDevice Vendor: " << device.getInfo<CL_DEVICE_VENDOR>() << std::endl;
			std::cout << "\t\tDevice Max Compute Units: " << device.getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>() << std::endl;
			std::cout << "\t\tDevice Global Memory: " << device.getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>() << std::endl;
			std::cout << "\t\tDevice Max Clock Frequency: " << device.getInfo<CL_DEVICE_MAX_CLOCK_FREQUENCY>() << std::endl;
			std::cout << "\t\tDevice Max Allocateable Memory: " << device.getInfo<CL_DEVICE_MAX_MEM_ALLOC_SIZE>() << std::endl;
			std::cout << "\t\tDevice Local Memory: " << device.getInfo<CL_DEVICE_LOCAL_MEM_SIZE>() << std::endl;
			std::cout << "\t\tDevice Available: " << device.getInfo< CL_DEVICE_AVAILABLE>() << std::endl;
		}
		std::cout << std::endl;

	}
}
*/
void OpenCLPlayground::test() {

}
