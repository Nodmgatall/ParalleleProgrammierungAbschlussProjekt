#define GLFW_INCLUDE_GLU
//https://www.youtube.com/watch?v=H20stuPG-To
#include "Util/vec3.hpp"
#include "particle.hpp"
#include "Tests/tester.hpp"
#include "physics.hpp"

#include <iostream>
#include <ostream>
#include <vector>

#include <cstdlib>

#include <random>
#include <array>
#include <utility>
#include <SDL2/SDL.h>
#include <stdlib.h>




const int SCREEN_WIDTH  = 1000;
const int SCREEN_HEIGHT = 1000;

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

void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y)
{
    //Setup the destination rectangle to be at the position we want
    SDL_Rect dst;
    dst.x = x;
    dst.y = y;
    //Query the texture to get its width and height to use
    SDL_QueryTexture(tex, NULL, NULL, &dst.w, &dst.h);
    dst.w = 16;
    dst.h = 16;
    SDL_RenderCopy(ren, tex, NULL, &dst);

}


int main(int argc, char** argv)
{
    // Vec3<double> test_vector(5,5,5);
    // Vec3<double> lol_2(5,5,5);
    // Vec3<double> rofl = test_vector + lol_2;

    Particle particle;

    // particle.generateRandomParticle(
    //         std::make_pair<double,double>(500000.0,100000000.0),
    //         std::make_pair<double,double>(1.0,50.0),
    //         std::make_pair<double,double>(1.0,5000),
    //         std::make_pair<double,double>(1.0,5000));

    // test_vector.display();
    // rofl.display();
    // particle.printParticle(0);
    // std::cout << rofl.getLength() << std::endl << std::endl;

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
    SDL_Texture* earth = loadTexture("Green_triangle.bmp", ren);
    SDL_Texture* sun = loadTexture("Green_triangle.bmp", ren);

    // void test_grav();

    //Particle particle;
    particle.createParticle(Vec3<double>(0,0,0),
                             Vec3<double>(0,0,0),
                             1.98855 * pow(10, 30),
                             10.0);
    particle.createParticle(Vec3<double>(149598261000,0,0),
                             Vec3<double>(0,29780,0),
                             5.972 * pow(10, 24),
                             10.0);

    unsigned long i = 0;
    unsigned long number_of_objects = strtoul(argv[1],NULL,0);
    // Vec3<double> start = Vec3<double>(0,149600000000,0);
    // Vec3<double> end;
    unsigned long id = 1;
    double dt = 0.000024;
    double scale = 5000000000;

    std::vector<SDL_Texture*>objects;;

    for(unsigned long i = 0; i < number_of_objects; i++)
    {
        objects.push_back(loadTexture("Green_triangle.bmp", ren));
        particle.generateRandomParticle(149598261000 * 20, 40000,5.972 * pow(10, 24) * 6, 100000000000000000);
    } 
    
     while(i< 100000)
    //while(i < 6)
    //while(particle.getPostion(id).getLength() > 10)
    {
        // if(i % 1 == 0)
        // {
        //     particle.printParticle(0);
        //     particle.printParticle(1);
        // }
        applyGravity(particle, id,dt);
        particle.move_Object(id,dt);
        particle.writePositionToFile("test.txt");
        particle.printParticle(id);
        // particle.getPostion(id).display();
        // First clear the renderer
        SDL_RenderClear(ren);
        //Draw the texture
        renderTexture(earth,ren,particle.getPostion(1).getX()/scale +500,particle.getPostion(1).getY()/scale + 500);
        renderTexture(sun,ren,particle.getPostion(0).getX()/scale + 500 ,particle.getPostion(0).getY()/scale + 500);
        //Update the screent

        for (unsigned long i = 0; i < number_of_objects; i++)
        {
            applyGravity(particle,i +2,dt);
            particle.move_Object(i + 2,dt);

            renderTexture(objects[i],ren,particle.getPostion(i+2).getX()/scale + 500 ,particle.getPostion(i+2).getY()/scale + 500);

        }
        SDL_RenderPresent(ren);
        SDL_Delay(5);
        //Take a quick break after all that hard work
        
        i++;
        
    }

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();

    //first_test();
    //test_grav();

	return EXIT_SUCCESS;
}
