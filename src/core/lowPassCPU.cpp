#include "lowPassCPU.h"
#include <iostream>
#include <chrono>

using namespace std;
using namespace std::chrono;

GPUImgProc::LowPassCPU::LowPassCPU(unsigned char * aData, int aWidth, int aHeight)
{
	this->width = aWidth;
	this->height = aHeight;

	this->ping = new float[width*height];
	setData(aData);
}

void GPUImgProc::LowPassCPU::setData(unsigned char* aData) {
	if (data) delete data;
	data = new float[width*height];
	for (int i = 0; i < width*height; ++i) {
		data[i] = (float)aData[i];
	}
}

void GPUImgProc::LowPassCPU::horizontalPass(int radius)
{
	float scale = 1.0f / (2.0f*radius + 1.0f);

	//fill out of bounds with zeros
	for (int j = 0; j < height; ++j) {
		for (int i = 0; i < radius + 1; ++i) {
			int currentInd = i + j*width;
			ping[currentInd] = 0;
		}
	}

	//fill out of bounds with zeros
	for (int j = 0; j < height; ++j) {
		for (int i = width - radius; i < width; ++i) {
			int currentInd = i + j*width;
			ping[currentInd] = 0;
		}
	}

	//Leave the edges for now
	for (int j = 0; j < height; ++j) {
		float* dPtr = &data[j*width];
		float* oPtr = &ping[j*width];
		for (int pass = 0; pass < numPasses; ++pass) {
			for (int i = radius; i < width - radius; ++i) {
				float res = 0;
				for (int r = -radius; r < radius; ++r) {
					res += *(dPtr + i + r);
				}
				res *= scale;
				*(oPtr + i) = res;
			}
			std::swap(dPtr, oPtr);
		}
	}
	if (numPasses % 2 != 0) {
		std::swap(data, ping); 
	}
}

//Straightforward implementation of transpose.
void GPUImgProc::LowPassCPU::transposeData() { 
	for (int j = 0; j < height; ++j) {
		for (int i = 0; i < width; ++i) {
			int currentInd = i + j*width;
			//row 1 should all be column 1.
			//last pixel on row 1 should be last pixel in column 1.
			//First assume NxN image
			int transposeInd = j + i*width;
			ping[transposeInd] = data[currentInd];
		}
	}
	std::swap(ping, data);
}
void  GPUImgProc::LowPassCPU::transpose(float* __restrict a, float * __restrict b, int width, int height, int strideA, int strideB)
{
	__assume(height <= TRANSPOSE_BLOCK_SIZE);
	__assume(width <= TRANSPOSE_BLOCK_SIZE);
	for (int j = 0; j < height; ++j) {
		for (int i = 0; i < width; ++i) {
			int index = i + j*strideA;
			int indexT = j + i*strideB;
			b[indexT] = a[index];
		}
	}
}
void GPUImgProc::LowPassCPU::recursiveTranspose(float * __restrict a, float * __restrict b, int widthA, int heightA, int strideA, int strideB)
{
	if (heightA <= TRANSPOSE_BLOCK_SIZE && widthA <= TRANSPOSE_BLOCK_SIZE) {
		transpose(a, b, widthA, heightA, strideA, strideB);
	}
	else if (heightA > widthA) { //Split along height in A
		float* aTop = a;
		float* aBot = a + heightA*strideA / 2;

		//splitting along height in A means splitting along width in B
		float* bLeft = b;
		float* bRight = b + heightA / 2;

		recursiveTranspose(aTop, bLeft, widthA, heightA / 2, strideA, strideB);
		recursiveTranspose(aBot, bRight, widthA, heightA / 2, strideA, strideB);
	}
	else { //Split along width in A
		float* aLeft = a;
		float* aRight = a + widthA / 2;

		//splitting along width in A means splitting along height in B
		float* bTop = b;
		float* bBot = b + widthA*strideB/ 2;

		recursiveTranspose(aLeft, bTop, widthA / 2, heightA, strideA,strideB);
		recursiveTranspose(aRight, bBot, widthA / 2, heightA, strideA, strideB);
	}
}

void GPUImgProc::LowPassCPU::horizontalPassCumulative(const int radius)
{

	//fill out of bounds with zeros
	for (int j = 0; j < height; ++j) {
		for (int i = 0; i < radius + 1; ++i) {
			int currentInd = i + j*width;
			ping[currentInd] = 0;
		}
	}

	//fill out of bounds with zeros
	for (int j = 0; j < height; ++j) {
		for (int i = width - radius; i < width; ++i) {
			int currentInd = i + j*width;
			ping[currentInd] = 0;
		}
	}

	for (int i = 0; i < height; ++i) {
		horizontalPassCumulativeRow(i, radius);
	}
	if (numPasses % 2 != 0) {
		std::swap(data, ping);
	}
}


void GPUImgProc::LowPassCPU::horizontalPassCumulativeRow(const int row, const int radius)
{
	float scale = 1.0f / (2.0f*radius + 1.0f);
	float* dPtr = &data[row*width];
	float* oPtr = &ping[row*width];
	for (int pass = 0; pass < numPasses; pass++) {
		
		//Sum for first pixel
		float sum = 0;
		for (int r = -radius; r < radius; ++r) {
			sum += *(dPtr + radius + r);
			//sum += data[radius + r + row * width];
		}

		//Leave the edges for now
		for (int i = radius; i < width - radius; ++i) {
			*(oPtr+i) = sum*scale;
			if (*(oPtr + i) < 0 ) {
				int p  = 0; 
			}
			sum += *(dPtr + i + radius ) - *(dPtr + i - radius); //remove leftmost value, add rightmost value
			//ping[currentInd] = sum*scale; // TODO Change this to ping once vert pass is implemnented
			//sum += data[i + radius + 1 + row*width] - data[i - radius + row*width];
		}
		std::swap(dPtr, oPtr);
	}
}

GPUImgProc::LowPassCPU::~LowPassCPU()
{
	delete data; 
	delete ping;
}
void GPUImgProc::LowPassCPU::execute()
{
		high_resolution_clock::time_point t1 = high_resolution_clock::now();

		int radius = 25;
		
		//Run functionality
		horizontalPassCumulative(radius);
		recursiveTranspose(data, ping, width, height, width, height);
		std::swap(data, ping);
		horizontalPassCumulative(radius);
		recursiveTranspose(data, ping, width, height, width, height);
		std::swap(data, ping);


		high_resolution_clock::time_point t2 = high_resolution_clock::now();

		auto duration = duration_cast<microseconds>(t2 - t1).count();

		cout << duration / 1000.0f << "ms" << endl;
}

void GPUImgProc::LowPassCPU::executeReference()
{
	for (size_t i = 0; i < 1; i++)
	{
		high_resolution_clock::time_point t1 = high_resolution_clock::now();

		int radius = 25;
		//Run functionality
		horizontalPass(radius);
		transposeData();
		horizontalPass(radius);
		transposeData();
		high_resolution_clock::time_point t2 = high_resolution_clock::now();

		auto duration = duration_cast<microseconds>(t2 - t1).count();

		cout << duration / 1000.0f << "ms" << endl;
	}
}

float * GPUImgProc::LowPassCPU::getDataF()
{
	return data;
}
unsigned char * GPUImgProc::LowPassCPU::getDataC()
{
	if (!outputC) outputC = new unsigned char[width*height];
	for (int i = 0; i < width*height; ++i) {
		outputC[i] = (unsigned char)data[i];
	}
	return outputC;
}