#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "vec3.hpp"
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <string>

class Camera {
  public:
    SDL_Rect get_rect();
    int *m_screen_width;
    int *m_screen_height;

    double m_x_rot_deg;
    double m_field_of_view;
    float m_dist_to_draw_plane;
    glm::vec3 m_position;
    glm::vec3 m_orientation;
    glm::vec3 x_axis;
    glm::vec3 y_axis;
    glm::vec3 z_axis;
    glm::vec3 m_plane_vec;
    float m_draw_radius;
    Camera();
    glm::vec3 get_positon();
    double get_size_on_plane(glm::vec3 object_position, glm::vec3 vector_to_plane);
    void init(int *screen_width, int *screen_height, double field_of_view);
    unsigned long get_distance_to_camera(glm::vec3 object_position);
    glm::vec4 calculate_position_on_draw_plane(glm::vec3 object_position);
    glm::vec2 get_screen_center();
    double get_rotation_angle();
    void move(glm::vec3 move_vector);
    void rotate(glm::quat rot_quad);
    void calculate_rotation_quat_from_drag(glm::vec2 mouse_rel);
    void to_start_position();

    float calculate_draw_radius();
};
#endif
