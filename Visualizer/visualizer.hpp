#ifndef VISUALIZER_HPP
#define VISUALIZER_HPP

#include "vec3.hpp"
#include "resource_manager.hpp"
#include "camera.hpp"
#include <SDL2/SDL.h>
#include <SDL_ttf.h>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <glm/glm.hpp>

class Visualizer
{
    private:
    
        std::vector<std::vector<glm::vec3 > > m_object_positions;
        std::vector<std::vector<glm::vec3 > > m_object_velocities;
        std::vector<std::vector<double> > m_object_masses;
        std::vector<std::vector<double> > m_object_radiuses;
        std::vector<std::vector<unsigned long> >m_object_ids;
        std::vector<double> m_iteration_dts;
        std::vector<std::vector<glm::u64vec3>> m_collision_data;
        std::vector<std::vector<glm::vec3>> m_collision_points;
        std::map<unsigned long, std::vector<std::pair<SDL_Color, glm::vec3>>> m_trajectory_lines;
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
        bool m_draw_dt;
        bool m_draw_time_simulated;
        double m_x_rot_deg;
        double m_y_rot_deg;
        std::vector<std::map<unsigned long, unsigned long>>  m_object_id_maps;
        std::map<unsigned long,unsigned long> m_line_draw_active;
        std::map<unsigned long,double> m_grav_range_draw_active;
        std::set<unsigned long> m_display_data_active;
        double m_time_simulated;
        unsigned long m_scale;
        unsigned long m_iteration_number;
        ResourceManager m_resource_manager;

        SDL_Renderer *m_renderer;
        SDL_Window *m_window;
        SDL_Rect m_camera;
        Camera m_camara_object;
        SDL_Rect m_background;
        TTF_Font *m_font;

        void create_info_window(unsigned long object_id);
        void init_SDL();
        void load_object_data_from_file(std::string filepath);
        void load_textures();
        void load_fonts();
        void render_texture(SDL_Texture *tex, int x, int y, int z, int p_width, int p_height);
        void main_loop();
        void pause_loop();
        void update();
        void draw_all_trajectory_lines();
        void generate_trajectory_line(unsigned long obj_id);
        void handle_console_input(std::string input);
        void clear_trajectory_lines();
        void draw_text(std::string text, int pos_x, int pos_y, SDL_Color color);
        void display_data(unsigned long particle_id);
        void draw_data();
        void display_grav_range(unsigned long id, double min_force);
        void draw_object_circle(unsigned long id);
        void display_all_grav_ranges();
        std::string format_seconds_to_time(double time);
        void load_particle_archive_from_file(std::string filename);
        SDL_Color set_color_gradient_for_speed(double speed, double max_speed, SDL_Color lowest_speed_color, SDL_Color middle_speed_color, SDL_Color highest_speed_color);

        void draw_vector(glm::vec3 pos_vec, glm::vec3 vec);
        bool is_all_digits(char *text);
        void draw_collison_marks();
        void calculate_collision_points();
        void draw_all_object_circles();
        void set_color(SDL_Color);

    
    public:
        
        Visualizer();
        Visualizer(int screen_height, int sreen_width);
        Visualizer(std::string filename);
        ~Visualizer();
        
        void draw_main_loop();
};

#endif
