#pragma once
#include <string>
#include <SDL.h>

namespace GPUImgProc {

	class PNGViewer {
	public:
		PNGViewer();
		~PNGViewer(); //Explicit destructor

		PNGViewer(const PNGViewer&) = delete; //Disallow the copy constructor
		PNGViewer& operator=(const PNGViewer&) = delete; //Disallow copy assign

		PNGViewer(PNGViewer&&) = delete; //Disallow move constructor
		PNGViewer& operator=(PNGViewer&&) = delete; //Disallow move assign


		unsigned char* getData();
		unsigned char* getDataSingleChannel();

		void setData(unsigned char* data, int width = 0, int height = 0);
		void setDataFromFile(std::string& filename);
		void show();
		void showWaitForEsc();

		int getWidth() { return w; }
		int getHeight() { return h; }

	private:
		void createWindow();
		void destroyWindow();
		void destroyTextures();
		void generateTexture();

		unsigned error = { 0 };
		unsigned char* image = nullptr;
		unsigned w = { 0 }, h = { 0 } , x = { 0 } , y = { 0 };
		SDL_Window* window = nullptr;
		SDL_Event event = { 0 };
		int done = { 0 };
		size_t jump = 1;
		SDL_Rect logo_dst = { 0 };
		SDL_Renderer* renderer = nullptr;
		SDL_Texture* png_texture = nullptr;

	};
}
