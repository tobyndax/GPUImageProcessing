/*
LodePNG Examples

Copyright (c) 2005-2012 Lode Vandevenne

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

*/


/*
Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions :

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software.If you use this software
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.

2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.

3. This notice may not be removed or altered from any source
distribution.
* /

/*
!!! This is a modified version of the LodePNG SDL Example updated to use SDL2 and C++ classes !!!
The code is modified in multiple locations! Compare to https://raw.githubusercontent.com/lvandeve/lodepng/master/examples/example_sdl.cpp to see what is changed.
*/


#include <lodepng.h>
#include <PNGViewer.h>
#include <SDL.h>

namespace GPUImgProc {

	PNGViewer::PNGViewer() {

	}
	PNGViewer::~PNGViewer()
	{
		if (image) delete image;
		destroyWindow();
		destroyTextures();
	}

	unsigned char * GPUImgProc::PNGViewer::getData()
	{
		return image;
	}

	unsigned char * GPUImgProc::PNGViewer::getDataSingleChannel()
	{
		unsigned char * newData = new unsigned char[w*h];
		for (int i = 0; i < w*h; ++i) {
			newData[i] = image[i * 4 + 0];
		}
		return newData;
	}



	//Load data for the viewer. Data is an unsigned char. Data will be interpreted as rgba (0xff000000 red, 0x00ff0000 green, 0x0000ff00 blue, 0x000000ff alpha)
	// TODO Fix, so one can set RGBA / Gray mode.
	// TODO How to handle different types of input? 
	void PNGViewer::setData(unsigned char* data, int width , int height)
	{
		w =  width == 0 ? w : width;
		h = height == 0 ? h : height; 
		size_t n = width*height*4; 
		if (image) delete image;
		image = new unsigned char[4*n]();
		for (int i = 0; i < w*h; ++i) {
			image[i * 4 + 0] = data[i];
			image[i * 4 + 1] = data[i];
			image[i * 4 + 2] = data[i];
			image[i * 4 + 3] = 255; // no alpha for this one
		}
	}

	void PNGViewer::setDataFromFile(std::string& filename) {
		/*load the PNG in one function call*/
		error = lodepng_decode32_file(&image, &w, &h, filename.c_str());
		/*stop if there is an error*/
		if (error)
		{
			printf("decoder error %u: %s\n", error, lodepng_error_text(error));
		}
	}

	void PNGViewer::generateTexture(){

		if (png_texture) {
			destroyTextures();
		}

		SDL_Surface* png_surface;
		if ((png_surface = SDL_CreateRGBSurface(0, (int)w, (int)h, 32, 0, 0, 0, 0)) == 0) {
			//Failure
			printf("Error generating the png_surface");
			return;
		}
		/*color the pixels of the PNG file*/
		for (y = 0; y + jump - 1 < h; y += jump)
			for (x = 0; x + jump - 1 < w; x += jump)
			{
				int checkerColor;
				Uint32* bufp;
				Uint32 r, g, b, a;

				/*get RGBA components*/
				r = image[4 * y * w + 4 * x + 0]; /*red*/
				g = image[4 * y * w + 4 * x + 1]; /*green*/
				b = image[4 * y * w + 4 * x + 2]; /*blue*/
				a = image[4 * y * w + 4 * x + 3]; /*alpha*/

												  /*make translucency visible by placing checkerboard pattern behind image*/
				checkerColor = 191 + 64 * (((x / 16) % 2) == ((y / 16) % 2));
				r = (a * r + (255 - a) * checkerColor) / 255;
				g = (a * g + (255 - a) * checkerColor) / 255;
				b = (a * b + (255 - a) * checkerColor) / 255;

				/*give the color value to the pixel of the screenbuffer*/
				bufp = (Uint32 *)png_surface->pixels + (y * png_surface->pitch / 4) / jump + (x / jump);
				*bufp = 65536 * r + 256 * g + b;
			}

		png_texture = SDL_CreateTextureFromSurface(renderer, png_surface);
		//Cleanup
		SDL_FreeSurface(png_surface);
	}

	void PNGViewer::createWindow() {
		//Select window size
		if (w / 1024 >= jump) jump = w / 1024 + 1;
		if (h / 1024 >= jump) jump = h / 1024 + 1;

		/*init SDL*/
		if (SDL_Init(SDL_INIT_VIDEO) < 0)
		{
			printf("Error, SDL video init failed\n");
			return;
		}
		window = SDL_CreateWindow("Press Esc to close the window and continue",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			w / jump, h / jump,
			SDL_WINDOW_RESIZABLE);

		if (!window)
		{
			printf("Error, no SDL screen\n");
			return; 
		}

		renderer = SDL_CreateRenderer(window, -1, 0);

	}
	
	void PNGViewer::show(){
		if (!image) {
			printf("Error, must initialize image before showing");
		}
	}
	
	void PNGViewer::showWaitForEsc() {
		if (!image) {
			printf("Error, must initialize image before showing");
		}
		int cw, ch;
		if (!window) createWindow();  //If there is no window, create it.
		SDL_GetWindowSize(window, &cw, &ch);
		if (cw != w/jump || ch != h/jump) SDL_SetWindowSize(window, w/jump, h/jump); //If image size changed, change window size
		generateTexture();

		/*pause until you press escape and meanwhile redraw screen*/
		done = 0;
		while (done == 0)
		{
			while (SDL_PollEvent(&event))
			{
				if (event.type == SDL_QUIT) done = 2;
				else if (SDL_GetKeyboardState(NULL)[SDLK_ESCAPE]) done = 2;
				else if (event.type == SDL_KEYDOWN) done = 1; /*press any other key for next image*/
			}
			// Render the entire logo to the center of the screen:
			logo_dst.x = (w / 2) - (w / 2);
			logo_dst.y = (h / 2) - (h / 2);
			logo_dst.w = w;
			logo_dst.h = w;
			SDL_RenderCopy(renderer, png_texture, NULL, &logo_dst);

			SDL_RenderPresent(renderer);
			SDL_Delay(5); /*pause 5 ms so it consumes less processing power*/
		}
	}
	
	void PNGViewer::destroyWindow() {
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
	}
	void PNGViewer::destroyTextures() {
		SDL_DestroyTexture(png_texture);
	}
	
}
