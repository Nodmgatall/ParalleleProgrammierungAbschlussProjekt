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
    to_start_position();
}

void Camera::move(glm::vec3 move_vector) {
    m_position += move_vector;
}

void Camera::to_start_position() {

    m_position = glm::vec3(*m_screen_width / 2, *m_screen_height / 2, 2*( m_dist_to_draw_plane));
    m_orientation = glm::vec3(0, 0, 1) * m_dist_to_draw_plane;
    m_plane_vec = glm::vec3(1, 0, 0) * m_dist_to_draw_plane;
    ;
}

void Camera::rotate(glm::quat rot_quat) {
    m_orientation = glm::normalize((m_orientation) * rot_quat) * m_dist_to_draw_plane;
}

unsigned long Camera::get_distance_to_camera(glm::vec3 object_position) {
    return length((m_position - object_position));
}

double Camera::get_size_on_plane(glm::vec3 object_position, glm::vec3 vector_to_plane_pos) {
    glm::vec3 camera_to_object_vector = m_position - object_position;
    return glm::length(camera_to_object_vector) / glm::length(vector_to_plane_pos);
}

void Camera::calculate_rotation_quat_from_drag(glm::vec2 mouse_rel) {
    /*
    // std::cout << "mouse_pssepspe" << to_string(mouse_pos_last - get_screen_center()) << " / "
    //          << to_string(mouse_pos_cur - get_screen_center()) << std::endl;
    glm::vec2 pos_last = mouse_pos_last - get_screen_center();
    glm::vec2 pos_cur = mouse_pos_cur - get_screen_center();

    glm::vec2 norm_pos_last = glm::normalize(pos_last);
    glm::vec2 norm_pos_cur = glm::normalize(pos_cur);

    float radius_sphere = glm::length(get_screen_center());
    // std::cout << "SPHERAE RA  "<<radius_sphere << std::endl;

    float length_x_y_cur = glm::length(pos_last) / radius_sphere;
    float length_x_y_last = glm::length(pos_cur) / radius_sphere;
    // std::cout << "lengts: cur: " << length_x_y_cur << " last: "  << length_x_y_last << std::endl;
    // std::cout << "lengts: cur: " << pow(length_x_y_last,2) << " last: "  <<
    // pow(length_x_y_cur,2) << std::endl;

    glm::vec3 norm_pos_last_3D =
        glm::vec3(norm_pos_last.x, -norm_pos_last.y, sqrt(1 - pow(length_x_y_last, 2)));
    glm::vec3 norm_pos_cur_3D =
        glm::vec3(norm_pos_cur.x, -norm_pos_cur.y, sqrt(1 - pow(length_x_y_cur, 2)));

    // std::cout << "3D last: " << to_string(norm_pos_last_3D) << std::endl;
    // std::cout << "3D cur: " << to_string(norm_pos_cur_3D) << std::endl;
    glm::vec3 rotation_axis = cross(m_plane_vec, m_orientation);
    float angle = glm::dot(norm_pos_last_3D, norm_pos_cur_3D);
    */

    // float x_angle = (mouse_rel.x/1000);
    float y_angle = mouse_rel.y / 1000;
    float x_angle = mouse_rel.x / 1000;


    if (abs(mouse_rel.x)  > abs(mouse_rel.y)) {
    //    rotate(glm::quat(cos(x_angle), sin(m_plane_vec / 2.f)));
    std::cout << "X_ANGLE: " << x_angle << std::endl;
    } else {
        rotate(glm::quat(cos(y_angle), sin(glm::cross(m_orientation,m_plane_vec) / 2.f)));
    std::cout << "Y_ANGLE: " << y_angle << std::endl;
    }
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

glm::vec2 Camera::get_screen_center() {
    return glm::vec2(*m_screen_width / 2, *m_screen_height / 2);
}
