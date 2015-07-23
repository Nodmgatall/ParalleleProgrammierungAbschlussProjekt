
#include "visualizer.hpp"
#include "resource_manager.hpp"
#include "vec3.hpp"
#include <SDL2/SDL.h>
#include <iostream>
#include <string>
#include <math.h>
#include <sstream>
#include <fstream>

const unsigned c_scroll_velo = 100;

void Visualizer::main_loop(double dt = 1)
{
    while(m_running == true)
    {
        update();
        draw_main_loop(dt);
        m_iteration_number++;
        if(m_iteration_number == m_object_positions.size())
        {
            m_iteration_number = 0;
        }
        SDL_Delay(10);
        //std::cout << lol++ << std::endl;
    }
    m_resource_manager.shutdown();
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

void Visualizer::pause_loop()
{
    while(m_pause == true)
    {
        update();
    }
}

void Visualizer::update()
{
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
            case SDL_QUIT:
                m_running = false;
                m_pause = false;
                break;

            case SDL_MOUSEWHEEL:
                std::cout << event.wheel.y <<  std::endl;
                m_scale += event.wheel.y * 100000000; 
                break;

            case SDL_KEYUP:

                switch(event.key.keysym.sym)
                {
                    case SDLK_DOWN:
                        m_camera.y -= c_scroll_velo;
                        break;

                    case SDLK_UP:
                        m_camera.y += c_scroll_velo;
                        break;

                    case SDLK_LEFT:
                        m_camera.x -= c_scroll_velo;
                        break;

                    case SDLK_RIGHT:
                        m_camera.x += c_scroll_velo;
                        break;

                    case SDLK_ESCAPE:
                        m_running = false;
                        break;

                    case SDLK_s:
                        if(m_pause == true)
                        {
                            m_iteration_number++;
                            draw_main_loop(1);
                        }
                        break;

                    case SDLK_p:
                        m_pause = !m_pause;
                        pause_loop();
                        break;

                    case SDLK_r:
                        m_iteration_number = 0;
                        draw_main_loop(1);
                        break;
                }
        }
    }
}

Visualizer::Visualizer()
{
    m_pause = false;
    m_running = true;
    m_iteration_number = 0;
    m_screen_width = 1600;
    m_screen_height = 900;
    m_scale = 5000000000;
    load_object_data_from_file("test.txt");
    init_SDL();
    load_textures();
    main_loop(1);
}

Visualizer::Visualizer(std::string filename)
{
    m_pause = false;
    m_running = true;
    m_iteration_number = 0;
    m_screen_width = 1600;
    m_screen_height = 900;
    m_camera = {-m_screen_width/2, m_screen_height/2, m_screen_width, m_screen_height};
    m_scale = 5000000000;
    load_object_data_from_file(filename);
    init_SDL();
    load_textures();
    main_loop(1);
}

Visualizer::~Visualizer()
{
    SDL_Quit();
}

void Visualizer::draw_main_loop(double dt)
{
    SDL_RenderClear(m_renderer);
    //std::cout << m_iteration_number << std::endl;
    for(unsigned long index = 0; index < m_object_positions[m_iteration_number].size(); ++index)
    {
        render_texture(m_resource_manager.get_texture("Triangle_Green"),
                (int)(m_object_positions[m_iteration_number][index].getX()/m_scale - m_camera.x),
                (int)(m_object_positions[m_iteration_number][index].getY()/m_scale + m_camera.y),
                (int)m_object_positions[m_iteration_number][index].getZ()/m_scale);
    }
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
    dst.w = 16;
    dst.h = 16;
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

void Visualizer::load_object_data_from_file(std::string filepath)
{
    std::ifstream file(filepath);
    std::string s;
    unsigned long it_number = 0;
    std::vector<Vec3<double>> it_vector;
    //std::string c;
    if(file.is_open())
    {
        while(getline(file,s))
        {
            if (s == ">")
            {
                std::cout << "new it "<< it_number << std::endl;
                m_object_positions.push_back(it_vector);
                //std::cout << it_vector.size() << std::endl;
                //std::cout << m_object_positions.size() << std::endl;
                it_vector.clear();
                it_number++;
            }
            else 
            {
                //std::cout << s << std::endl;
                std::string x,y,z;
                std::stringstream ss(s);
                getline(ss,x,' ');
                getline(ss,y,' ');
                getline(ss,z,' ');
                it_vector.push_back(Vec3<double>(std::stod(x),std::stod(y),std::stod(z)));
                //std::cout << it_vector.back().toString() << std::endl;

            }
        }
        file.close();
        std::cout << "loading done" << std::endl;
    }
    else
    {
        std::cout << "file could not be opened" << std::endl;
    }
}
