
#include "visualizer.hpp"
#include "resource_manager.hpp"
#include "vec3.hpp"
#include <SDL2/SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include <string>
#include <math.h>
#include <sstream>
#include <fstream>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

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
    ;}

Visualizer::Visualizer(std::string filename)
{
    m_pause = false;
    m_running = true;
    m_iteration_number = 0;
    m_screen_width = 900;
    m_screen_height = 900;
    m_input = false;
    m_perspective = 'x';
    m_console_is_open = false;
    m_draw_it_number = false;
    m_rotation_active = false;
    m_x_rot_deg = 0.0;
    m_y_rot_deg = 0.0;
    m_camera = {-m_screen_width/2, m_screen_height/2, m_screen_width, m_screen_height};
    m_scale = 5000000000;
    load_object_data_from_file(filename);
    init_SDL();
    SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
    load_textures();
    load_fonts();
    main_loop(1);
}

Visualizer::~Visualizer()
{
    SDL_Quit();
}

void Visualizer::init_SDL()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }

    if(TTF_Init() == -1)
    {
        std::cout << "TFF_Init failure: " << TTF_GetError() << std::endl;
    }
    m_window = SDL_CreateWindow("Visualizer", 100, 100, m_screen_width, m_screen_height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
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

void Visualizer::draw_text(std::string text, int pos_x, int pos_y, SDL_Color color)
{
    SDL_Rect rect{pos_x, pos_y,12 * (int)text.length(),12};
    SDL_Surface *font_surf = TTF_RenderText_Blended(m_resource_manager.get_font("data-latin"), text.c_str(), color);
    SDL_Texture *font_tex = SDL_CreateTextureFromSurface(m_renderer, font_surf);
    SDL_RenderCopy(m_renderer, font_tex, nullptr, &rect);
    SDL_FreeSurface(font_surf);
    SDL_DestroyTexture(font_tex);
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
                    return;
                }
                break;
            case SDL_WINDOWEVENT_RESIZED:
                std::cout << "HERE WE ARE AND IT SHOULD WORK" << std::endl;
                SDL_GetWindowSize(m_window, &m_screen_width, &m_screen_height);
                //m_screen_width = event.window.data1;
                //m_screen_height = event.window.data2;
                break;

            case SDL_MOUSEMOTION:
                if(m_rotation_active)
                {
                    //VERSUCH VON VIEW ROTATION ...i
                    //int x_rel =  event.motion.xrel;
                    //int y_rel = event.motion.yrel;
                    //std::cout << x_rel << std::endl;
                    //std::cout << y_rel << std::endl;
                    //if(x_rel != 0)
                    //{
                    //    m_x_rot_deg += 0.01 * x_rel;
                    //}
                    //if(y_rel != 0)
                    //{
                     //   m_y_rot_deg += 0.1 * y_rel;
                    //}
                        //m_x_rot_deg += event.motion.x ;
                }
                break;

            case SDL_KEYUP:

                switch(event.key.keysym.sym)
                {
                    case SDLK_LCTRL:
                        m_rotation_active = !m_rotation_active;
                        break;

                    case SDLK_RETURN:
                        if(m_input == false)
                        {
                            m_input = true;
                        }
                        else
                        {
                            if(command == "")
                            {
                                break;
                            }
                            handle_console_input(command);
                            m_input = false;
                            command = "";
                        }
                        break;

                    case SDLK_BACKSPACE:
                        if(m_input && command != "")
                        {
                            command.erase(command.end()-1);
                        }
                        break;

                    case SDLK_BACKQUOTE:
                        m_console_is_open = !m_console_is_open;
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
                        if(!m_input)
                        {
                            m_iteration_number = 0;
                            draw_main_loop(1);
                        }
                        break;

                    case SDLK_x:
                        m_perspective = 'x';
                        break;

                    case SDLK_y:
                        m_perspective = 'y';
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
    token = strtok(c_input, " ");
    token_vector.push_back(token);

    while((token = strtok(NULL, " ")) != NULL)
    {
        token_vector.push_back(token);
    } 

    if(strcmp(token_vector[0], "draw_ids") == 0 ||
            strcmp(token_vector[0], "dis") == 0)
    {
        m_draw_ids = !m_draw_ids;
        return;
    }

    if(strcmp(token_vector[0], "help") == 0)
    {
        return;
    }

    if(strcmp(token_vector[0], "draw_it_number") == 0 || strcmp(token_vector[0], "din") == 0)
    {
        m_draw_it_number = !m_draw_it_number;
        return;
    }

    if(strcmp(token_vector[0], "display_data") == 0 || strcmp(token_vector[0], "dd") == 0)
    {
        unsigned long particle_id;

        if(token_vector.size() > 2)
        {
            std::cout << "Error: To many arguments. Needed arguments: 1 Given arguments: " << token_vector.size() - 1 << std::endl;
            return;
        }

        if(token_vector.size() < 2)
        {
            std::cout << "Error: Missing arguments. Needed arguments: 1 Given arguments: " << token_vector.size() - 1 << std::endl;
            return;
        }

        if(!is_all_digits(token_vector[1]))
        {
            std::cout << "Error: argument 1 has non digit parts." <<         std::endl;
            return;
        }

        particle_id = atoi(token_vector[1]);

        if(particle_id >= m_object_positions.size())
        {
            std::cout << "Error: Argument 1 is too large, should be smaller than " << m_object_positions.size() - 1 << std::endl;
            return;
        }

        auto it = m_display_data_active.find(particle_id);
        if(it != m_display_data_active.end())
        {
            m_display_data_active.erase(it);
            return;
        }

        m_display_data_active.insert(particle_id);

        return;
    }

    if(strcmp(token_vector[0],"clear_displayed_data") == 0
            || strcmp(token_vector[0],"cdd") == 0)
    {
        m_display_data_active.clear();
    }

    if(strcmp(token_vector[0], "jmp") == 0)
    {
        if(token_vector.size() > 2)
        {
            std::cout << "Error: To many arguments. Needed arguments: 1 Given arguments: " << token_vector.size() - 1 << std::endl;
            return;
        }

        if(!is_all_digits(token_vector[1]))
        {
            std::cout << "Error: argument 1 has non digit parts." << std::endl;
            return;
        }

        unsigned long new_it_number = atoi(token_vector[1]);

        if(new_it_number > m_object_positions.size())
        {
            std::cout << "Error: iteration number to large. Max iteration is: " << m_object_positions.size() - 1 << std::endl;
            return;
        }

        m_iteration_number = new_it_number;

        return;
    }


    if(strcmp(token_vector[0], "show_trajectory") == 0 
            || strcmp(token_vector[0], "st") == 0)
    {
        unsigned long particle_id;   

        if(token_vector.size() > 2)
        {
            std::cout << "Error: To many arguments. Needed arguments: 1 Given arguments: " << token_vector.size() - 1 << std::endl;
            return;
        }

        if(!is_all_digits(token_vector[1]))
        {
            std::cout << "Error: argument 1 has non digit parts." << std::endl; 
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
        return;

    }

    if(strcmp(token_vector[0], "clear_trajectory") == 0 
            || strcmp(token_vector[0], "ct") == 0)
    {
        clear_trajectory_lines();
        return;
    }

    std::cout << "no such command known: " << command << std::endl;
    return;

}

bool Visualizer::is_all_digits(char *text)
{

    int string_length = strlen(text); 

    for(int i = 0; i < string_length - 2; i++)
    {
        if(!isdigit(text[i]))
        {
            return false;
        }
    }
    return true;

}

void Visualizer::draw_main_loop(double dt)
{
    int pos_x;
    int pos_y;
    // int pos_z;
    SDL_RenderClear(m_renderer);
    //std::cout << m_iteration_number << std::endl;
    render_texture(m_resource_manager.get_texture("Background"),
            0,0,0,1920,1080);
    draw_all_trajectory_lines();

    for(unsigned long index = 0; index < m_object_positions[m_iteration_number].size(); ++index)
    {
        if(m_perspective == 'x')
        {
            pos_y = (m_object_positions[m_iteration_number][index].getY()/ m_scale) + m_camera.y;
        }
        else
        {
            pos_y = (m_object_positions[m_iteration_number][index].getZ()/ m_scale) + m_camera.y;

        }
        pos_x = (int)(m_object_positions[m_iteration_number][index].getX()/m_scale) - m_camera.x;

        render_texture(m_resource_manager.get_texture("Triangle_Green"), pos_x, pos_y, 0, 16, 16);

        if(m_draw_ids)
        {
            draw_text(std::to_string(index), pos_x + 18 , pos_y - 18, {255, 255, 255, 255});
        }
    } 

    if(m_draw_it_number)
    {
        draw_text("Iteration: " + std::to_string(m_iteration_number), 50,50, {255, 255, 255, 255});
    }

    draw_data();

    if(m_console_is_open)
    {
        render_texture(m_resource_manager.get_texture("ConBack"),100,100,0,450,600);
    }


    if(m_input && command != "")
    {
        draw_text(command, 100, 100, {255, 255 ,255 ,255});
    }
    SDL_RenderPresent(m_renderer);
}

void Visualizer::draw_all_trajectory_lines()
{
    for(auto it = m_line_draw_active.begin(); it != m_line_draw_active.end(); ++it)
    {
        draw_trajectory_line(*it);
    }
    return;
}

void Visualizer::display_grav_range(unsigned long id, double min_force)
{

}

void Visualizer::display_data(unsigned long particle_id)
{
    Vec3<double> pos = m_object_positions[m_iteration_number][particle_id];
    pos.setX(pos.getX() / 149597870700.0);
    pos.setY(pos.getY() / 149597870700.0);
    pos.setZ(pos.getZ() / 149597870700.0);

    double dist = pos.getLength();

    int pos_x;
    int pos_y;

    std::vector<SDL_Point> line;

    //std::cout << m_object_positions[index][obj_id].getX() / m_scale - m_camera.x << std::endl;
    //std::cout << m_object_positions[index][obj_id].getY() / m_scale + m_camera.y << std::endl;
    if(m_perspective == 'x')
    {
        pos_y = m_object_positions[m_iteration_number][particle_id].getY() / m_scale + m_camera.y;
    }
    else
    {
        pos_y = m_object_positions[m_iteration_number][particle_id].getZ() / m_scale + m_camera.y; 
    }
    pos_x = (int)((m_object_positions[m_iteration_number][particle_id].getX())/ m_scale - m_camera.x);

    //Vec3<double> current_velo = m_object_positions[m_iteration_number + 1][particle_id] - m_object_positions[m_iteration_number][particle_id];
    SDL_Color color = {176, 33, 38, 255};
    draw_text("Coordinates:", pos_x + 20, pos_y, color);
    draw_text(pos.toString(),pos_x + 60, pos_y + 12, color);
    draw_text("Distance to Sun:", pos_x + 20, pos_y + 24, color);
    draw_text(std::to_string(dist) + " AU", pos_x + 60, pos_y + 36, color);
    draw_text("Velocity:", pos_x + 20, pos_y + 48, color);
    draw_text(std::to_string(m_object_velocities[m_iteration_number][particle_id].getLength()) + " m/s", pos_x + 60, pos_y + 60, color);
    draw_text("Mass:", pos_x + 20, pos_y + 72, color);
    draw_text(std::to_string(m_object_masses[m_iteration_number][particle_id] / 100000000000000) + " x 10^15", pos_x + 60, pos_y + 84, color);
    draw_text("Radius:", pos_x + 20, pos_y + 96, color);
    draw_text(std::to_string(m_object_radiuses[m_iteration_number][particle_id] / 100000000000000) + " x 10^15", pos_x + 60, pos_y + 108, color);
}

void Visualizer::draw_data()
{
    for(auto it = m_display_data_active.begin(); it != m_display_data_active.end(); ++it)
    {
        display_data(*it);
    }
}

void Visualizer::draw_trajectory_line(unsigned long particle_id)
{
    double coord2;
    std::vector<SDL_Point> line;
    for(unsigned long index = 0;  index < m_object_positions.size(); index++)
    {
        //std::cout << m_object_positions[index][obj_id].getX() / m_scale - m_camera.x << std::endl;
        //std::cout << m_object_positions[index][obj_id].getY() / m_scale + m_camera.y << std::endl;
        if(m_perspective == 'x')
        {
            coord2 = m_object_positions[index][particle_id].getY();
        }
        else
        {
            coord2 = m_object_positions[index][particle_id].getZ(); 
        }
        SDL_Point new_point = {
            (int)(((m_object_positions[index][particle_id].getX())/ m_scale) - m_camera.x),
            (int)(((coord2)/ m_scale ) + m_camera.y)};
        line.push_back(new_point);
    }
    SDL_Point *line_array = &line[0];
    SDL_RenderDrawPoints(m_renderer, line_array, m_object_positions.size());    
}

void Visualizer::clear_trajectory_lines()
{
    m_line_draw_active.clear();
}

void Visualizer::load_textures()
{
    m_resource_manager.load_texture("Triangle_Green",
            "Textures/Green_triangle.bmp",
            m_renderer);
    m_resource_manager.load_texture("Background",
            "Textures/Background.bmp",
            m_renderer);
    m_resource_manager.load_texture("ConBack","Textures/ConsoleBackground.bmp", m_renderer);
}

void Visualizer::load_fonts()
{
    m_resource_manager.load_font("data-unifon", "Fonts/data-unifon.ttf",32);
    m_resource_manager.load_font("data-latin", "Fonts/data-latin.ttf",32);
    m_resource_manager.load_font("aero", "Fonts/Aero.ttf",64);

}

void Visualizer::render_texture(SDL_Texture *texture, int x, int y, int z,int p_width, int p_height)
{
    //Setup the destination rectangle to be at the position we w64t
    SDL_Rect dst;
    dst.x = x;
    dst.y = y;
    dst.w = p_width;
    dst.h = p_height;
    SDL_RenderCopy(m_renderer, texture, NULL, &dst);
}

void Visualizer::load_object_data_from_file(std::string filepath)
{
    std::ifstream file(filepath);
    std::string s;
    unsigned long it_number = 0;
    std::vector<Vec3<double> > it_pos_vector;
    std::vector<Vec3<double> > it_velo_vector;
    std::vector<double> it_masses;
    std::vector<double> it_radiuses;
    //std::string c;
    if(file.is_open())
    {
        while(getline(file,s))
        {
            if (s == ">")
            {
                std::cout << "new it "<< it_number << std::endl;
                m_object_positions.push_back(it_pos_vector);
                m_object_velocities.push_back(it_velo_vector);
                m_object_masses.push_back(it_masses);
                m_object_radiuses.push_back(it_radiuses);
                //std::cout << it_vector.size() << std::endl;
                //stT::cout << m_object_positions.size() << std::endl;
                it_pos_vector.clear();
                it_masses.clear();
                it_velo_vector.clear();
                it_radiuses.clear();
                it_number++;
            }
            else 
            {
                //std::cout << s << std::endl;
                std::string x,y,z,velo_x,velo_y,velo_z,mass, radius;
                std::stringstream ss(s);
                getline(ss,x,' ');
                getline(ss,y,' ');
                getline(ss,z,' ');
                getline(ss,velo_x,' ');
                getline(ss,velo_y,' ');
                getline(ss,velo_z,' ');
                getline(ss,mass,' ');
                getline(ss,radius,' ');


                it_pos_vector.push_back(Vec3<double>(std::stod(x),std::stod(y),std::stod(z)));
                it_velo_vector.push_back(Vec3<double>(std::stod(velo_x),std::stod(velo_y), std::stod(velo_z)));
                it_masses.push_back(std::stod(mass));
                it_radiuses.push_back(std::stod(radius));
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
