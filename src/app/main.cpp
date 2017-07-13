#include <PNGViewer.h>
#include <sdl.h> // Need this for SDL_Quit at exit
#include <lowPassCPU.h>


int actualMain() {
	GPUImgProc::PNGViewer* pngLoader = new GPUImgProc::PNGViewer();
	std::string pngPath = "..\\..\\testdata\\lenaGrayLarge.png";
	pngLoader->setDataFromFile(pngPath);

	unsigned char* data;
	data = pngLoader->getDataSingleChannel();

	GPUImgProc::LowPassCPU *lowPCPU = new GPUImgProc::LowPassCPU(data, pngLoader->getWidth(), pngLoader->getHeight());

	lowPCPU->execute();
	unsigned char* newData = lowPCPU->getDataC();
	pngLoader->setData(reinterpret_cast<unsigned char*>(newData), pngLoader->getWidth(), pngLoader->getHeight());

	pngLoader->showWaitForEsc();

	lowPCPU->setData(data);

	lowPCPU->executeReference();
	newData = lowPCPU->getDataC();
	pngLoader->setData(reinterpret_cast<unsigned char*>(newData), pngLoader->getWidth(), pngLoader->getHeight());

	pngLoader->showWaitForEsc();

	delete pngLoader;
	return 0;
}

int main(int argc, char * argv[])
{

	actualMain();

	atexit(SDL_Quit);
	return 0;
}
 