#include "camera.hpp"
#include <string>
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
Camera::Camera() {
    x = 0;
    y = 0;
    z = 0;

    x_axis = {1, 0, 0};
    y_axis = {0, 1, 0};
    z_axis = {0, 0, 1};
}

void Camera::init(int screen_width, int screen_height, double field_of_view) {
    m_field_of_view = field_of_view;
    m_screen_width = screen_width;
    m_screen_height = screen_height;
    x = screen_width / 2;
    y = screen_height / 2;

    m_dist_to_draw_plane = (screen_width / 2) / tan(glm::radians(m_field_of_view / 2));
    m_orientation = glm::vec3(0, 0, -1) * m_dist_to_draw_plane;
    m_plane_vec = glm::vec3(0, 1, 0) * m_dist_to_draw_plane;
    z = 100000000000;
    m_position = glm::vec3(x, y, z);
}

void Camera::move(glm::vec3 move_vector) {
    x += move_vector.x;
    y += move_vector.y;
    z += move_vector.z;
}

void Camera::rotate(double new_angle) {


    float angle = glm::radians(new_angle);
    glm::quat rot_y(cos(angle/2),0,sin(angle/2),0);
    m_orientation = glm::normalize(m_orientation) * (rot_y) * glm::length(m_orientation);
   /* uuu
    glm::quat orientation_quat(0, m_orientation);
    glm::quat rotation_quat(glm::cos(angle_in_rad), 0, 1 * glm::sin(angle_in_rad), 0);
    glm::quat rot_2(glm::cos(angle_in_rad), 1 * glm::sin(angle_in_rad), 0, 0);
    glm::quat new_orientation = rotation_quat * orientation_quat;
    m_orientation.x = new_orientation.x;
    m_orientation.y = new_orientation.y;
    m_orientation.z = new_orientation.z;
    glm::quat lol(0, m_plane_vec);
    glm::quat new_lol = rotation_quat * lol;
    m_plane_vec.x = new_lol.x;
    m_plane_vec.y = new_lol.y;
    m_plane_vec.z = new_lol.z;
    m_orientation = glm::normalize(m_orientation) * m_dist_to_draw_plane;
    m_plane_vec = glm::normalize(m_plane_vec) * m_dist_to_draw_plane;
*/
    }

unsigned long Camera::get_distance_to_camera(glm::vec3 object_position) {
    return length((glm::vec3(x, y, z) - object_position));
}

double Camera::get_size_on_plane(glm::vec3 object_position, glm::vec3 vector_to_plane_pos) {
    glm::vec3 camera_to_object_vector = m_position - object_position;
    return glm::length(camera_to_object_vector) / glm::length(vector_to_plane_pos);
}

glm::vec4 Camera::calculate_position_on_draw_plane(glm::vec3 object_position) {
    // calculate distance for object planet
    glm::vec3 camera_to_object_vector = object_position - m_position;
    glm::vec3 distance_from_center_of_plane_on_plane;
    double winkel_rad = acos(glm::dot(m_orientation, camera_to_object_vector) /
                             (glm::length(camera_to_object_vector) * glm::length(m_orientation)));
    float f = glm::length(m_orientation) / cos(winkel_rad);
    glm::vec3 from_direction_to_plane_pos =
        (glm::normalize(camera_to_object_vector) * f) - m_orientation;

    return glm::vec4(from_direction_to_plane_pos + m_position, winkel_rad);
}
