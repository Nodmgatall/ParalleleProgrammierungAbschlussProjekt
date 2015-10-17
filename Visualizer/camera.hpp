#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "vec3.hpp"
#include <SDL2/SDL.h>
#include <string>

class Camera
{
    private: 
        void move(Vec3<double> move_vector);
        void rotate_x(double angle);
        Vec3<double> get_positon();
        double get_rotation_angle();
        SDL_Rect get_rect();
        int m_screen_width;
        int m_screen_height;

    public:
        unsigned long get_distance_to_camera(Vec3<double> object_position);
        Vec3<double> calculate_position_on_draw_plane(Vec3<double> object_position);
        Camera();
        void init(int screen_width, int screen_height, double field_of_view);
        int h;
        int w;
        double x;
        double y;
        double z;
        Vec3<double> m_orientation;
        double m_field_of_view;
        double m_dist_to_draw_plane;

};
#endif
