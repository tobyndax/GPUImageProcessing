#pragma once


namespace GPUImgProc{

	class LowPassCPU {

	public:
		LowPassCPU(unsigned char* data, int width, int height);
		~LowPassCPU();
		void execute();
		float* getDataF(); 
		unsigned char* getDataC();


	private:
		int numPasses = 3;
		void horizontalPass(int radius);
		void verticalPass(int radius);
		void horizontalPassCumulative(int radius);
		void horizontalPassCumulativeRow(const int row ,const int radius);
		int width = 0;
		int height = 0;
		float* data = nullptr;
		float* ping = nullptr;
		float* pong = nullptr;
		unsigned char* outputC = nullptr;

	};
}
