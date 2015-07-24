
#include "visualizer.hpp"
#include "resource_manager.hpp"
#include "vec3.hpp"
#include <SDL2/SDL.h>
#include <iostream>
#include <string>
#include <math.h>
#include <sstream>
#include <fstream>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

const unsigned c_scroll_velo = 100;
std::string command = "";
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
                m_scale += event.wheel.y * 100000000;
                if(m_scale == 0)
                {
                    m_scale = 100000000;
                }
                break;

            case SDL_TEXTINPUT:
                if(m_input == true)
                {
                    command += event.text.text;
                    std::cout << command << std::endl;
                }
                break;

            case SDL_KEYUP:

                switch(event.key.keysym.sym)
                {
                    case SDLK_RETURN:
                        if(m_input == false)
                        {
                            m_input = true;
                        }
                        else
                        {
                            if(command == "") break;
                            handle_console_input(command);
                            m_input = false;
                            command = "";

                        }
                        break;
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
                        if(m_input) break;
                        if(m_pause == true)
                        {
                            m_iteration_number++;
                            draw_main_loop(1);
                        }
                        break;

                    case SDLK_p:
                        if(m_input) break;
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

void Visualizer::handle_console_input(std::string input)
{
    char *c_input = (char *)input.c_str();
    std::vector<char*> token_vector;
    char *token;
    int string_length = strlen(c_input);
    token = strtok(c_input, " ");
    token_vector.push_back(token);

    while((token = strtok(NULL, " ")) != NULL)
    {
        token_vector.push_back(token);
    } 
    for(unsigned i = 0; i <  token_vector.size(); i++)
    {
        std::cout << token_vector[i] << std::endl;
    }
    if(strcmp(token_vector[0], "clearTrajectory") == 0 
            || strcmp(token_vector[0], "ct") == 0)
    {
        clear_trajectory_lines();
    }


    if(strcmp(token_vector[0], "showTrajectory") == 0 
            || strcmp(token_vector[0], "st") == 0)
    {
                std::cout << token_vector.size() << "TEST "<< std::endl;
        bool is_all_digit = true;
        unsigned long particle_id;

        if(token_vector.size() > 2)
        {
            std::cout << "Error: To many arguments. Needed arguments: 1 Given arguments: " << token_vector.size() - 1 << std::endl;
            return;
        }

        for(int i = 0; i < string_length - 2; i++)
        {
            is_all_digit = isdigit(token_vector[1][i]);
            std::cout << is_all_digit << std::endl;
        }
        if(is_all_digit)
        {
            std::cout << "Error: Argument 1 is not a digit" << std::endl;
            return;  
        }
        particle_id = atoi(token_vector[1]);

        auto it = m_line_draw_active.find(particle_id);
        if(it != m_line_draw_active.end())
        {
            m_line_draw_active.erase(it);
            return;
        }

        if(particle_id >= m_object_positions.size())
        {
            std::cout << "Error: Argument 1 is too large, should be smaller than " << m_object_positions.size() - 1 << std::endl;
            return;
        }

        m_line_draw_active.insert(particle_id);

    }
    return;

}

Visualizer::Visualizer()
{
    m_pause = false;
    m_running = true;
    m_iteration_number = 0;
    m_screen_width = 900;
    m_screen_height = 900;
    m_input = false;
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
    m_screen_width = 900;
    m_screen_height = 900;
    m_input = false;
    m_camera = {-m_screen_width/2, m_screen_height/2, m_screen_width, m_screen_height};
    m_scale = 5000000000;
    load_object_data_from_file(filename);
    init_SDL();
    SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
    load_textures();
    main_loop(1);
}

Visualizer::~Visualizer()
{
    SDL_Quit();
}
void Visualizer::clear_trajectory_lines()
{
    m_line_draw_active.clear();
}

void Visualizer::draw_all_trajectory_lines()
{
    for(auto it = m_line_draw_active.begin(); it != m_line_draw_active.end(); ++it)
    {
        draw_trajectory_line(*it);
    }
    return;
}

void Visualizer::draw_trajectory_line(unsigned long particle_id)
{
    std::vector<SDL_Point> line;
    for(unsigned long index = 0;  index < m_object_positions.size(); index++)
    {
        //std::cout << m_object_positions[index][obj_id].getX() / m_scale - m_camera.x << std::endl;
        //std::cout << m_object_positions[index][obj_id].getY() / m_scale + m_camera.y << std::endl;

        SDL_Point new_point = {
            (int)((m_object_positions[index][particle_id].getX() + 8)/ m_scale - m_camera.x),
            (int)((m_object_positions[index][particle_id].getY() + 8 )/ m_scale + m_camera.y)};
        line.push_back(new_point);
    }
    SDL_Point *line_array = &line[0];
    SDL_RenderDrawPoints(m_renderer, line_array, m_object_positions.size());    
}

void Visualizer::draw_main_loop(double dt)
{
    SDL_RenderClear(m_renderer);
    //std::cout << m_iteration_number << std::endl;
    render_texture(m_resource_manager.get_texture("Background"),
            0,0,0,1920,1080);
    draw_all_trajectory_lines();
    for(unsigned long index = 0; index < m_object_positions[m_iteration_number].size(); ++index)
    {
        render_texture(m_resource_manager.get_texture("Triangle_Green"),
                (int)(m_object_positions[m_iteration_number][index].getX()/m_scale - m_camera.x),
                (int)(m_object_positions[m_iteration_number][index].getY()/m_scale + m_camera.y),
                (int)m_object_positions[m_iteration_number][index].getZ()/m_scale,16,16);
    }
    SDL_RenderPresent(m_renderer);
}

void Visualizer::load_textures()
{
    m_resource_manager.load_texture("Triangle_Green",
            "Textures/Green_triangle.bmp",
            m_renderer);
    m_resource_manager.load_texture("Background",
            "Textures/Background.bmp",
            m_renderer);
}

void Visualizer::render_texture(SDL_Texture *texture, int x, int y, int z,int p_width, int p_height)
{
    //Setup the destination rectangle to be at the position we want
    SDL_Rect dst;
    dst.x = x;
    dst.y = y;
    dst.w = p_width;
    dst.h = p_height;
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
