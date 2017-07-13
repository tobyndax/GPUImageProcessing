#pragma once


namespace GPUImgProc{

	class LowPassCPU {

	public:
		LowPassCPU(unsigned char* data, int width, int height);
		~LowPassCPU();
		void execute();
		void executeReference();
		float* getDataF(); 
		unsigned char* getDataC();
		void setData(unsigned char* data);


	private:
		const static int TRANSPOSE_BLOCK_SIZE = 16;
		void horizontalPass(int radius);
		//void verticalPass(int radius);
		void horizontalPassCumulative(int radius);
		void horizontalPassCumulativeRow(const int row ,const int radius);
		void transposeData();
		void recursiveTranspose(float* a, float* b, int widthA, int heightA, int strideA,int strideB);
		//void recursiveTranspose(int a, int b, int widthA, int heightA, int strideA);

		int numPasses = 3;
		int width = 0;
		int height = 0;
		float* data = nullptr;
		float* ping = nullptr;
		float* pong = nullptr;
		unsigned char* outputC = nullptr;

	};
}
