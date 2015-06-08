#define GLFW_INCLUDE_GLU
//https://www.youtube.com/watch?v=H20stuPG-To
#include <iostream>
#include <ostream>

#include <cstdlib>

#include <random>
#include <array>
#include <utility>
#include <SDL2/SDL.h>

#include "vec3.hpp"
#include "particle.hpp"

const int SCREEN_WIDTH  = 640;
const int SCREEN_HEIGHT = 480;

// class Trangle
// {
// 	SDL_Rect triangle_rect;
// 	Triangle(double new_pos_x, double new_pos_y, &)
// 	{
// 		triangle_rect.x = new_pos_x;
// 		triangle_rect.y = new_pos_y;
// 		triangle_rect.w = 32;
// 		triangle_rect.h = 32;
// 	}
		
// }

void logSDLError(std::ostream &os, const std::string &msg){
	os << msg << " error: " << SDL_GetError() << std::endl;
}

SDL_Texture* loadTexture(const std::string &file, SDL_Renderer *ren){
	//Initialize to nullptr to avoid dangling pointer issues
	SDL_Texture *texture = nullptr;
	//Load the image
	SDL_Surface *loadedImage = SDL_LoadBMP(file.c_str());
	//If the loading went ok, convert to texture and return the texture
	if (loadedImage != nullptr){
		texture = SDL_CreateTextureFromSurface(ren, loadedImage);
		SDL_FreeSurface(loadedImage);
		//Make sure converting went ok too
		if (texture == nullptr){
			logSDLError(std::cout, "CreateTextureFromSurface");
		}
	}
	else {
		logSDLError(std::cout, "LoadBMP");
	}
	return texture;
}

void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y){
	//Setup the destination rectangle to be at the position we want
	SDL_Rect dst;
	dst.x = x;
	dst.y = y;
	//Query the texture to get its width and height to use
	SDL_QueryTexture(tex, NULL, NULL, &dst.w, &dst.h);
	dst.w = 32;
	dst.h = 32;
	SDL_RenderCopy(ren, tex, NULL, &dst);

}


int main(int argc, char** argv)
{
	Vec3<double> test_vector(5,5,5);
	Vec3<double> lol_2(5,5,5);
	Vec3<double> rofl = test_vector + lol_2;
	
	Particle particle;

	particle.generateRandomParticle(
		std::make_pair<double,double>(500000.0,100000000.0),
		std::make_pair<double,double>(1.0,50.0),
		std::make_pair<double,double>(1.0,5000),
		std::make_pair<double,double>(1.0,5000));

	test_vector.display();
	rofl.display();
	particle.printParticle(0);
	std::cout << rofl.getLength() << std::endl << std::endl;

	if (SDL_Init(SDL_INIT_VIDEO) != 0){
	std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
	return 1;
	}
	
	SDL_Window *win = SDL_CreateWindow("Hello World!", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	
	if (win == nullptr)
	{
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}

	SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (ren == nullptr)
	{
		SDL_DestroyWindow(win);
		std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}

	//std::string imagePath = getResourcePath("Lesson1") + "red_circle.bmp";
	SDL_Texture* triangle = loadTexture("Green_triangle.bmp", ren);

	for (int i = 0; i < 3; ++i){
	//First clear the renderer
	SDL_RenderClear(ren);
	//Draw the texture
	renderTexture(triangle,ren,120,12);
	//Update the screen
	SDL_RenderPresent(ren);
	//Take a quick break after all that hard work
	SDL_Delay(1000);
}

SDL_DestroyRenderer(ren);
SDL_DestroyWindow(win);
SDL_Quit();
	return EXIT_SUCCESS;
}