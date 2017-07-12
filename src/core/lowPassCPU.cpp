#include "lowPassCPU.h"
#include <iostream>
#include <chrono>

using namespace std;
using namespace std::chrono;

GPUImgProc::LowPassCPU::LowPassCPU(unsigned char * data, int width, int height)
{
	
	this->ping = new float[width*height];
	this->pong = new float[width*height];
	this->width = width;
	this->height = height;

	this->data = new float[width*height];
	for (int i = 0; i < width*height; ++i) {
		this->data[i] = (float)data[i];
	}

}

void GPUImgProc::LowPassCPU::horizontalPass(int radius)
{
	float scale = 1.0f / (2.0f*radius + 1.0f);

	//fill out of bounds with zeros
	for (int j = 0; j < height; ++j) {
		for (int i = 0; i < radius+1; ++i) {
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
		for (int pass = 0; pass < numPasses; ++pass) {
			for (int i = radius; i < width - radius; ++i) {
				int currentInd = i + j*width;
				float res = 0;
				for (int r = -radius; r < radius; ++r) {
					res += data[currentInd + r];
				}
				res *= scale;
				ping[currentInd] = res;
			}
			std::copy_n(&ping[j*width], width, &data[j*width]);
		}
	}
}

void GPUImgProc::LowPassCPU::verticalPass(int radius)
{
	//fill out of bounds with zeros
	for (int j = 0; j < radius + 1; ++j) {
		for (int i = 0; i < width; ++i) {
			int currentInd = i + j*width;
			ping[currentInd] = 0;
		}
	}

	//fill out of bounds with zeros
	for (int j = height-radius; j < height; ++j) {
		for (int i = 0; i < width; ++i) {
			int currentInd = i + j*width;
			ping[currentInd] = 0;
		}
	}

	float scale = 1.0f / (2.0f*radius + 1.0f);
	for (int j = radius; j < height - radius; ++j) {
		for (int pass = 0; pass < numPasses; ++pass) {
			for (int i = 0; i < width; ++i) {
				int currentInd = i + j*width;
				float res = 0;
				for (int r = -radius; r < radius; ++r) {
					res += data[currentInd + r*width];
				}
				res *= scale;
				ping[currentInd] = res; //TODO Fix
			}
		}
	}
}

void GPUImgProc::LowPassCPU::horizontalPassCumulative(const int radius)
{
	for (int i = 0; i < height; ++i) {
		horizontalPassCumulativeRow(i, radius);
	}
}


void GPUImgProc::LowPassCPU::horizontalPassCumulativeRow(const int row, const int radius)
{
	float scale = 1.0f / (2.0f*radius + 1.0f);

	//Sum for first pixel
	float sum = 0;
	for (int r = -radius; r < radius; ++r) {
		sum += data[radius + r + row * width];
	}

	//Leave the edges for now
	for (int i = radius; i < width - radius; ++i) {
		int currentInd = i + row * width;
		pong[currentInd] = sum*scale; // TODO Change this to ping once vert pass is implemnented
		sum += data[i + radius + 1 + row*width] - data[i - radius + row*width];
	}
}

GPUImgProc::LowPassCPU::~LowPassCPU()
{
	//Nothing needed here yet
}
void GPUImgProc::LowPassCPU::execute()
{
	for (size_t i = 0; i < 1; i++)
	{

		high_resolution_clock::time_point t1 = high_resolution_clock::now();

		int radius = 25;
		//Run functionality
		horizontalPass(radius);
		verticalPass(radius);
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