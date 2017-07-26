
__kernel void vector_add(__global const float4 *A, __global const float4 *B, __global float4 *C) {
	int i = get_global_id(0);
	C[i] = native_sqrt(native_powr(A[i],2) + native_powr(B[i],2));
	//C[i] = A[i] + B[i];
}