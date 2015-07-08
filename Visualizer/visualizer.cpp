
#include "visualizer.hpp"
#include "resource_manager.hpp"
#include <SDL2/SDL.h>
#include <iostream>
#include <string>

void main_loop()
{
    while(m_running == true)
    {
        update();
        draw();
    }

}

void update()
{
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        if(event.type == SDL_Quit)
        {
            m_running = false;
        }
        {

        }
        if(state[SDL_SCANCODE_ESC])
        {

        }
    }
}

void Visualizer::Visualizer()
{
    init_SDL();
}
void Visualizer::draw(unsigned long interation_number, double dt)
{
    for(unsigned long index = 0; index < m_object_positions[interation_number].size(); ++index)
    {
        render_texture(m_resource_manager.get_texture("Triangle_Green"),
                (int)m_object_positions[interation_number][index].getX(),
                (int)m_object_positions[interation_number][index].getY(),
                (int)m_object_positions[interation_number][index].getZ());
    }
}
void Visualizer::load_textures()
{
    m_resource_manager.load_texture("Triangle_Green",
            "Textures/Green_triangle.bmp",
            m_renderer); 
}
void Visualizer::render_texture(SDL_Texture *texture, int x, int y, int z)
{
    //Setup the destination rectangle to be at the position we want
    SDL_Rect dst;
    dst.x = x;
    dst.y = y;
    //Query the texture to get its width and height to use
    SDL_QueryTexture(texture, NULL, NULL, &dst.w, &dst.h);
    SDL_RenderCopy(m_renderer, texture, NULL, &dst);
}
void Visualizer::init_SDL()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }



    m_window = SDL_CreateWindow("Hello World!", 100, 100, m_screen_width, m_screen_height, SDL_WINDOW_SHOWN);

    if (m_window == nullptr)
    {
        std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (m_renderer == nullptr)
    {
        SDL_DestroyWindow(m_window);
        std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        exit(EXIT_FAILURE);
    }
}
