#ifndef VISUALIZER_HPP
#define VISUALIZER_HPP

#include "vec3.hpp"
#include "resource_manager.hpp"
#include <SDL2/SDL.h>
#include <SDL_ttf.h>
#include <vector>
#include <set>
#include <map>

class Visualizer
{
    private:
    
        std::vector<std::vector<Vec3<double> > > m_object_positions;
        std::vector<std::vector<Vec3<double> > > m_object_velocities;
        std::vector<std::vector<double> > m_object_masses;
        std::vector<std::vector<double> > m_object_radiuses;
        std::vector<std::vector<unsigned long> >m_object_ids;
        int m_screen_width;
        int m_screen_height;
        bool m_running;
        bool m_pause;
        bool m_input;
        char m_perspective;
        bool m_draw_ids;
        bool m_console_is_open;
        bool m_draw_it_number;
        bool m_rotation_active;
        bool m_draw_number_of_particles;
        double m_x_rot_deg;
        double m_y_rot_deg;
        std::map<unsigned long,unsigned long> m_line_draw_active;
        std::map<unsigned long,double> m_grav_range_draw_active;
        std::set<unsigned long> m_display_data_active;
        unsigned long m_scale;
        unsigned long m_iteration_number;
        ResourceManager m_resource_manager;

        SDL_Renderer *m_renderer;
        SDL_Window *m_window;
        SDL_Rect m_camera;
        SDL_Rect m_background;
        TTF_Font *m_font;

        void init_SDL();
        void load_object_data_from_file(std::string filepath);
        void load_textures();
        void load_fonts();
        void render_texture(SDL_Texture *tex, int x, int y, int z, int p_width, int p_height);
        void main_loop(double dt);
        void pause_loop();
        void update();
        void draw_all_trajectory_lines();
        void draw_trajectory_line(unsigned long obj_id, unsigned long draw_range);
        void handle_console_input(std::string input);
        void clear_trajectory_lines();
        void draw_text(std::string text, int pos_x, int pos_y, SDL_Color color);
        void display_data(unsigned long particle_id);
        void draw_data();
        void display_grav_range(unsigned long id, double min_force);
        void draw_object_circle(unsigned long id);
        void display_all_grav_ranges();

        bool is_all_digits(char *text);
    
    public:
        
        Visualizer();
        Visualizer(int screen_height, int sreen_width);
        Visualizer(std::string filename);
        ~Visualizer();
        
        void draw_main_loop(double dt);
};

#endif
