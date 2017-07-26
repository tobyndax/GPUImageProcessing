#include <iostream>
#include "lowPassGPU.h"
#include <cassert>
#define MAX_SOURCE_SIZE (0x100000)

using namespace cl;

LowPassGPU::LowPassGPU()
{
	//Do not perform any heavy lifting initialization.
	//Do heavy init in initOpenLC and uploadImage for timings.
}

LowPassGPU::~LowPassGPU()
{

}

void LowPassGPU::initOpenCL()
{
	cl_platform_id platform_id = NULL;
	cl_uint ret_num_devices;
	cl_uint ret_num_platforms;

	cl_int ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);

	ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, &ret_num_devices);

	// Create an OpenCL context (which device?) 
	context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);

	// Create a command queue (why?)
	command_queue = clCreateCommandQueue(context, device_id, 0, &ret);

	FILE *fp;
	char *source_str;
	::size_t source_size;

	fp = fopen("lowPassKernel.cl", "r");
	if (!fp) {
		fprintf(stderr, "Failed to load kernel.\n");
		exit(1);
	}
	source_str = (char*)malloc(MAX_SOURCE_SIZE);
	source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
	fclose(fp);

	// Create a program from the kernel source
	cl_program program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const ::size_t *)&source_size, &ret);
	assert(ret == 0);

	// Build the program
	ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
	if (ret != 0) {
		char buffer[10240];
		clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, NULL);
		fprintf(stderr, "CL Compilation failed:\n%s", buffer);
	}
	assert(ret == 0);
	// Create the OpenCL kernel
	lowPassKernel = clCreateKernel(program, "lowPass", &ret);
	transposeKernel= clCreateKernel(program, "transpose", &ret);
	assert(ret == 0);

	
}


void LowPassGPU::uploadImage(unsigned char* imPtr, int aWidth, int aHeight) {
	width = aWidth;
	height = aHeight;

	setData(imPtr); // copy data to local float ptr input.
	cl_int ret = 0;

					// Create memory buffers on the device for each input array
	input_mem_obj = clCreateBuffer(context, CL_MEM_READ_WRITE,
		width*height * sizeof(float), NULL, &ret);
	assert(ret == 0);

	//output buffer
	output_mem_obj = clCreateBuffer(context, CL_MEM_READ_WRITE,
		width*height * sizeof(float), NULL, &ret);

	assert(ret == 0);

	// Set the arguments of the kernel
	ret = clSetKernelArg(lowPassKernel, 0, sizeof(cl_mem), (void *)&input_mem_obj);
	assert(ret == 0);

	ret = clSetKernelArg(lowPassKernel, 1, sizeof(cl_mem), (void *)&output_mem_obj);
	assert(ret == 0);

	//Upload data to OpenCL memory.
	clEnqueueWriteBuffer(command_queue, input_mem_obj, CL_TRUE,0 , width*height * sizeof(float), input, NULL, NULL, NULL);

}

void LowPassGPU::swapBuffers() {

	std::swap(input_mem_obj, output_mem_obj);
	// Set the arguments of the kernel
	cl_int ret = clSetKernelArg(lowPassKernel, 0, sizeof(cl_mem), (void *)&input_mem_obj);
	assert(ret == 0);

	ret = clSetKernelArg(lowPassKernel, 1, sizeof(cl_mem), (void *)&output_mem_obj);
	assert(ret == 0);
}

void LowPassGPU::downloadData()
{
	if (output == nullptr) output = new float[width*height];
	//Upload data to OpenCL memory.
	clEnqueueReadBuffer(command_queue, output_mem_obj, CL_TRUE, 0, width*height * sizeof(float), output, NULL, NULL, NULL);
}

/*Copy data from unsigned char* to float input data
*/
void LowPassGPU::setData(unsigned char* aData) {
	if (input) delete input;
	input = new float[width*height];
	for (int i = 0; i < width*height; ++i) {
		input[i] = (float)aData[i];
	}
}

void LowPassGPU::transpose() {
	// Set the arguments of the kernel
	cl_int ret = clSetKernelArg(transposeKernel, 0, sizeof(cl_mem), (void *)&input_mem_obj);
	assert(ret == 0);

	ret = clSetKernelArg(transposeKernel, 1, sizeof(cl_mem), (void *)&output_mem_obj);
	assert(ret == 0);

	const ::size_t globalWorkSize = width*height;
	const ::size_t lws = 64;
	ret = clEnqueueNDRangeKernel(command_queue, transposeKernel, 1, NULL, &globalWorkSize, &lws, NULL, NULL, NULL);
	assert(ret == 0);
	
	swapBuffers();
}

void LowPassGPU::execute()
{
	for(int i = 0; i < 3; i++){
		const ::size_t globalWorkSize = width*height;
		const ::size_t lws = 64;
		cl_int ret = clEnqueueNDRangeKernel(command_queue, lowPassKernel, 1, NULL, &globalWorkSize, &lws, NULL, NULL, NULL);
		assert(ret == 0); 
		swapBuffers();
	}

	transpose();

	for (int i = 0; i < 3; i++) {
		const ::size_t globalWorkSize = width*height;
		const ::size_t lws = 64;
		cl_int ret = clEnqueueNDRangeKernel(command_queue, lowPassKernel, 1, NULL, &globalWorkSize, &lws, NULL, NULL, NULL);
		assert(ret == 0);
		swapBuffers();
	}

	transpose();
	swapBuffers();
	clFinish(command_queue); //Finish the clCommandqueue to not misrepresent the timings 
}


/* If you use this function to get the data, you are responsible for the deletion of the memory.
* Returns a pointer to a unsigned char version of the output data.
*/
unsigned char * LowPassGPU::getDataC()
{
	unsigned char * outputC = new unsigned char[width*height];
	for (int i = 0; i < width*height; ++i) {
		outputC[i] = (unsigned char)output[i];
	}
	return outputC;
}