#ifndef VISUALIZER_HPP
#define VISUALIZER_HPP

#include "vec3.hpp"
#include "resource_manager.hpp"
#include <SDL2/SDL.h>
#include <vector>
class Visualizer
{
    private:
    
        std::vector<std::vector<Vec3<double> > > m_object_positions;
        int m_screen_width;
        int m_screen_height;
        bool m_running;
        ResourceManager m_resource_manager;
        SDL_Renderer *m_renderer;
        SDL_Window *m_window;
        
        void init_SDL();
        void load_object_data_from_file();
        void load_textures();
        void render_texture(SDL_Texture *tex, int x, int y, int z);
        void main_loop();
        void update();
    
    public:
        
        Visualizer();
        Visualizer(int screen_height, int sreen_width);
        ~Visualizer();
        
        void draw(unsigned long interation_number, double dt);
};

#endif
