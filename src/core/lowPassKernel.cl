
__kernel void transpose(__global const float *input, __global float *output) {
	const int width = 2048;
	const int radius = 11;
	int idx = get_global_id(0);

	const int row = floor((float)idx/width);
	const int column = idx - row*width;

	output[row + column*width] = input[column + row*width];
}

__kernel void lowPass(__global const float *input, __global float *output) {
	const int width = 2048;
	const int radius = 11;
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