
__kernel void transpose_ref(__global const float *input, __global float *output) {
	const int width = 2048;
	const int radius = 25;
	int idx = get_global_id(0);
	int idy = get_global_id(1);

	output[idy + idx*width] = input[idx + idy*width];
}
//based on https://developer.apple.com/library/content/documentation/Performance/Conceptual/OpenCL_MacProgGuide/TuningPerformanceOntheGPU/TuningPerformanceOntheGPU.html#//apple_ref/doc/uid/TP40008312-CH22-SW7
__kernel void transpose(__global const float *input, __global float *output) {
	const int width = 2048;
	const int height = 2048;
	const int radius = 25;

	local float aux[16*16];
	
	int bx = get_group_id(0);
	int by = get_group_id(1);
	by = (by+bx)%get_num_groups(1); // Skew mapping

	int ix = get_local_id(0);
	int iy = get_local_id(1);

	//move input/output pointer
	input  += (bx*16) + (by*16)*width;
	output += (by*16) + (bx*16)*height;

	aux[iy + ix*16] = input[ix+iy*width];

	//Wait for all threads to finish
	barrier(CLK_LOCAL_MEM_FENCE);

	output[ix + iy*width] = aux[ix + iy*16];
}

__kernel void lowPass(__global const float *input, __global float *output) {
	const int width = 2048;
	const int radius = 25;
	int idx = get_global_id(0);
	int idy = get_global_id(1);

	const float scale = 1.0f / (2.0f*radius + 1.0f);

	int lowerBound = max(0, idx-radius);
	int upperBound = min(idx+radius, width-radius);
    float res = 0;
	
	for (int i = lowerBound; i < upperBound; i++){
		res += input[i + idy*width];	
	}

	output[idx + idy * width] = res*scale*(idx > radius)*(idx < width-radius);
}
/*
__kernel void lowPass(__global const float *input, __global float *output) {
	const int width = 2048;
	const int radius = 25;
	int idx = get_global_id(0);

	const int row = floor((float)idx/width);
	const int column = idx - row*width;


	const float scale = 1.0f / (2.0f*radius + 1.0f);

	int lowerBound = max(0,idx-radius);
	int upperBound = min(width+width*row,idx+radius);
    float res = 0;
	
	for (int i = lowerBound; i < upperBound; i++){
		res = res + input[i];	
	}

	output[idx] = res*scale*(column > radius)*(column < width-radius);
}
*/
__kernel void lowPassC(__global const float *input, __global float *output) {
	const int width = 2048;
	const int height = 2048;
	const int radius = 25;
	int idx = get_global_id(0);

	const int row = floor((float)idx/width);
	const int column = idx - row*width;

	const float scale = 1.0f / (2.0f*radius + 1.0f);

	int lowerBound = max(0,idx-radius*width);
	int upperBound = min(height*width - height*radius, idx + radius*width);
    float res = 0;
	
	for (int i = lowerBound; i < upperBound; i += width){
		res = res + input[i];	
	}

	output[idx] = res*scale*(row > radius)*(row < height-radius);
}