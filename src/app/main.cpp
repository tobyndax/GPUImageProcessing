#include <PNGViewer.h>
#include <sdl.h> // Need this for SDL_Quit at exit


int main(int argc, char * argv[])
{

	GPUImgProc::PNGViewer* pngLoader = new GPUImgProc::PNGViewer();
	std::string pngPath = "..\\..\\testdata\\lenaWithTrans.png";
	pngLoader->setDataFromFile(pngPath);
	pngLoader->showWaitForEsc();
	
	unsigned int width = 256;
	unsigned int height = 256;
	unsigned char* img = new unsigned char[width*height]();
	for (unsigned int i = 0; i < width; ++i) {
		for (unsigned int j = 0; j < height; ++j) {
			img[j*width + i] = (unsigned char)((i + j)/2);
		}
	}

	pngLoader->setData(img,width,height);
	pngLoader->showWaitForEsc();

	delete pngLoader;

	atexit(SDL_Quit);
	return 0;
}
 