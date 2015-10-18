#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "vec3.hpp"
#include <SDL2/SDL.h>
#include <string>
#include <glm/glm.hpp>

class Camera
{
    public:
        void move(glm::vec3 move_vector);
        void rotate(double angle);
        glm::vec3 get_positon();
        double get_rotation_angle();
        SDL_Rect get_rect();
        int m_screen_width;
        int m_screen_height;

        unsigned long get_distance_to_camera(glm::vec3 object_position);
        glm::vec4 calculate_position_on_draw_plane(glm::vec3 object_position);
        Camera();
        void init(int screen_width, int screen_height, double field_of_view);
        int h;
        int w;
        double x;
        double y;
        double z;
        double m_x_rot_deg;
        glm::vec3 m_position;
        glm::vec3 m_orientation;
        double m_field_of_view;
        float m_dist_to_draw_plane;
        double get_size_on_plane(glm::vec3 object_position, glm::vec3 vector_to_plane);
        glm::vec3 x_axis;
        glm::vec3 y_axis;
        glm::vec3 z_axis;

};
#endif
