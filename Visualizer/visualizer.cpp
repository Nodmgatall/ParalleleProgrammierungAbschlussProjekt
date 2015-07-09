
#include "visualizer.hpp"
#include "resource_manager.hpp"
#include <SDL2/SDL.h>
#include <iostream>
#include <string>

void Visualizer::main_loop(double dt = 1)
{
    int lol = 0;
    unsigned long interation_number = 0;
    while(m_running == true)
    {
        update();
        draw(interation_number, dt);
    std::cout << lol++ << std::endl;
    }
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

void Visualizer::update()
{
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        if(event.type == SDL_QUIT)
        {
            m_running = false;
        }
        else if(event.type == SDL_KEYUP)
        {
            switch(event.key.keysym.sym)
            {
                case SDLK_ESCAPE:
                    m_running = false;
                break;
            }
        }
    }
}

Visualizer::Visualizer()
{
    m_screen_width = 200;
    m_screen_height = 200;
    init_SDL();
    load_textures();
    main_loop(1);
}

Visualizer::~Visualizer(){}

void Visualizer::draw(unsigned long interation_number, double dt)
{
    SDL_RenderClear(m_renderer);
    render_texture(m_resource_manager.get_texture("Triangle_Green"),100,100,100);
    std::cout << "here1" << std::endl;
//    for(unsigned long index = 0; index != 0|| index < m_object_positions[interation_number].size(); ++index)
//    {
//        render_texture(m_resource_manager.get_texture("Triangle_Green"),
//                (int)m_object_positions[interation_number][index].getX(),
//                (int)m_object_positions[interation_number][index].getY(),
//                (int)m_object_positions[interation_number][index].getZ());
//    }
    std::cout << "here2" << std::endl;
    SDL_RenderPresent(m_renderer);
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
