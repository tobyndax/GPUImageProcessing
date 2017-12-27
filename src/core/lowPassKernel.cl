
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

__kernel void lowPassGPU(__global const float *input, __global float *output) {
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

//This kernel runs with global work size = #rows
__kernel void lowPassSimple(__global const float *input, __global float *output) {
	const int width = 2048;
	const int radius = 25;
	
	int y = get_global_id(0);
	global const float* inRow = input + y*width;
	global float* outRow = output + y*width;

	__local float aux1[2048];
	float sumA = 0.0f;
	float sum1 = 0.0f;

	// Left border
	int x = -radius;
	for (; x<=radius; x++) {
		// Here, sumA corresponds to segment 0..x+radius-1, update to 0..x+radius.
		sumA += inRow[x+radius]; sum1 += 1.0f;
		if (x >= 0) outRow[x] = sumA/sum1;
	}
	// x is RANGE+1 here
 
	// Internal pixels
	float k = 1.0f/(float)(2*radius+1);                  // Constant weight for internal pixels
	for (; x + radius < width; x++) {
		sumA -= inRow[x-radius-1];
		sumA += inRow[x+radius];
		outRow[x] = sumA * k;
	}
	// Right border
	for (; x < width; x++) {
		sumA -= inRow[x - radius - 1]; sum1 -= 1.0f;
		outRow[x] = sumA/sum1;
	}
}

//This kernel runs with global work size = #rows
__kernel void lowPassCPU(__global const float *input, __global float *output) {
	const int width = 2048;
	const int radius = 25;
	
	int y = get_global_id(0);
	global const float* inRow = input + y*width;
	global float* outRow = output + y*width;

	__local float aux1[2048];
    __local float aux2[2048];
	float sumA = 0.0f;
	float sum1 = 0.0f;

	// Left border
	int x = -radius;
	for (; x<=radius; x++) {
		// Here, sumA corresponds to segment 0..x+radius-1, update to 0..x+radius.
		sumA += inRow[x+radius]; sum1 += 1.0f;
		if (x >= 0) aux1[x] = sumA/sum1;
	}
	// x is RANGE+1 here
 
	// Internal pixels
	float k = 1.0f/(float)(2*radius+1);                  // Constant weight for internal pixels
	for (; x + radius < width; x++) {
		sumA -= inRow[x-radius-1];
		sumA += inRow[x+radius];
		aux1[x] = sumA * k;
	}
	// Right border
	for (; x < width; x++) {
		sumA -= inRow[x - radius - 1]; sum1 -= 1.0f;
		aux1[x] = sumA/sum1;
	}

	sumA = 0.0f;
	sum1 = 0.0f;

	// Left border
	x = -radius;
	for (; x<=radius; x++) {
		// Here, sumA corresponds to segment 0..x+radius-1, update to 0..x+radius.
		sumA += aux1[x+radius]; sum1 += 1.0f;
		if (x >= 0) aux2[x] = sumA/sum1;
	}
	// x is RANGE+1 here
 
	// Internal pixels
	k = 1.0f/(float)(2*radius+1);                  // Constant weight for internal pixels
	for (; x + radius < width; x++) {
		sumA -= aux1[x-radius-1];
		sumA += aux1[x+radius];
		aux2[x] = sumA * k;
	}
	// Right border
	for (; x < width; x++) {
		sumA -= aux1[x - radius - 1]; sum1 -= 1.0f;
		aux2[x] = sumA/sum1;
	}

	sumA = 0.0f;
	sum1 = 0.0f;

	// Left border
	x = -radius;
	for (; x<=radius; x++) {
		// Here, sumA corresponds to segment 0..x+radius-1, update to 0..x+radius.
		sumA += aux2[x+radius]; sum1 += 1.0f;
		if (x >= 0) output[x*width + y] = sumA/sum1;
	}
	// x is RANGE+1 here
 
	// Internal pixels
	k = 1.0f/(float)(2*radius+1);                  // Constant weight for internal pixels
	for (; x + radius < width; x++) {
		sumA -= aux2[x-radius-1];
		sumA += aux2[x+radius];
		output[x*width + y]  = sumA * k;
	}
	// Right border
	for (; x < width; x++) {
		sumA -= aux2[x - radius - 1]; sum1 -= 1.0f;
		output[x*width + y]  = sumA/sum1;
	}
}

// Vertical pass v3. Global work size: >= number of CPU cores.
kernel void lowPassCPUV(global const float * input,global float * output) {

  const int width = 2048;
  const int height = 2048;
  const int radius = 25;	
  // Number of rows to process in each work item (rounded up)
  int rowsPerItem = (height+get_global_size(0)-1)/get_global_size(0);
  int y0 = rowsPerItem * get_global_id(0);         // Update the range Y0..Y1-1
  int y1 = min(height, y0 + rowsPerItem);
  for (int y=y0; y<y1; y++) {
    // Accumulate into row y all rows in y-RANGE..y+RANGE intersected with 0..h-1
    int ya0 = max(0, y-radius);
    int ya1 = min(height, y+radius+1);
    float k = 1.0f/(float)(ya1-ya0);              // 1/(number of rows)
    global float * outRow = output + width*y;            // Output row
    for (int x=0; x<width; x++) outRow[x] = 0.0f;     // Zero output row
    for (int ya=ya0; ya<ya1; ya++) {
      global const float * inRow = input + width*ya;     // Input row
      for (int x=0; x<width; x++) outRow[x] += k * inRow[x];
    }
  }
}
