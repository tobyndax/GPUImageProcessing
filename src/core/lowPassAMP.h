#include <amp.h>

class LowPassAMP {
public:
	LowPassAMP();
	~LowPassAMP();

	void init();
	void uploadData(unsigned char* aData, int width, int height);
	void execute();

private:
	int width;
	int height;
	float* input;
	float* output;
};