#include "camera.hpp"
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <math.h>
#include <string>
Camera::Camera() {

    m_position = {0, 0, 0};

    x_axis = {1, 0, 0};
    y_axis = {0, 1, 0};
    z_axis = {0, 0, 1};
}

void Camera::init(int *screen_width, int *screen_height, double field_of_view) {
    m_field_of_view = field_of_view;
    m_screen_width = screen_width;
    m_screen_height = screen_height;
    m_dist_to_draw_plane = (*screen_width / 2) / tan(glm::radians(m_field_of_view / 2));
    m_draw_radius = calculate_draw_radius();
    to_start_position();
}

float Camera::calculate_draw_radius() {
    return sqrt(pow(m_dist_to_draw_plane, 2) +
                pow(sqrt(pow(*m_screen_width, 2) + pow(*m_screen_height, 2)),2));
}

void Camera::move(glm::vec3 move_vector) {
    m_position += move_vector;
}

void Camera::to_start_position() {

    m_position = glm::vec3(0,0, -80000000* (m_dist_to_draw_plane));
    m_orientation = glm::vec3(0, 0, 1) * m_dist_to_draw_plane;
    m_plane_vec = glm::vec3(0, 1, 0) * m_dist_to_draw_plane;
    ;
}

void Camera::rotate(glm::quat rot_quat) {
    m_orientation = glm::normalize((m_orientation)*rot_quat) * m_dist_to_draw_plane;
    m_plane_vec = glm::normalize((m_plane_vec)*rot_quat) * m_dist_to_draw_plane;
}

unsigned long Camera::get_distance_to_camera(glm::vec3 object_position) {
    return length((m_position - object_position));
}

double Camera::get_size_on_plane(glm::vec3 object_position, glm::vec3 vector_to_plane_pos) {
    glm::vec3 camera_to_object_vector = m_position - object_position;
    return glm::length(camera_to_object_vector) / glm::length(vector_to_plane_pos);
}
// depricated
void Camera::calculate_rotation_quat_from_drag(glm::vec2 mouse_rel) {
    float y_angle = mouse_rel.y / 1000;
    float x_angle = mouse_rel.x / 1000;

    if (abs(mouse_rel.x) > abs(mouse_rel.y)) {
            rotate(glm::quat(cos(x_angle), sin(m_plane_vec / 2.f)));
        std::cout << "X_ANGLE: " << x_angle << std::endl;
    } else {
        //rotate(glm::quat(cos(y_angle), sin(glm::cross(m_orientation, m_plane_vec) / 2.f)));
        std::cout << "Y_ANGLE: " << y_angle << std::endl;
    }
}

glm::vec4 Camera::calculate_position_on_draw_plane(glm::vec3 object_position) {
    // calculate distance for object planet
    glm::vec3 camera_to_object_vector = object_position - m_position;
    double winkel_rad =
        acos(
                glm::dot(
                    glm::normalize(m_orientation),
                    glm::normalize(camera_to_object_vector)
                    )
                );
    
    float f = m_dist_to_draw_plane / cos(winkel_rad);
if(object_position == glm::vec3(0,0,0))
    {
    std::cout << winkel_rad << std::endl;
    std::cout << to_string(m_position) << std::endl;
    std::cout << to_string(object_position) << std::endl;
    std::cout << to_string(camera_to_object_vector) << std::endl << std::endl;
    std::cout << "f: " << f << std::endl;
    }
    glm::vec3 from_direction_to_plane_pos = ((glm::normalize(camera_to_object_vector) * f ) - m_orientation) + glm::vec3(450,450,0);

    return glm::vec4(from_direction_to_plane_pos + m_position , winkel_rad);
}

glm::vec2 Camera::get_screen_center() {
    return glm::vec2(*m_screen_width / 2, *m_screen_height / 2);
}
