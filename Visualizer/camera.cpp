#include "camera.hpp"
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <math.h>
#include <string>

/*
 *   <==========init methods=========>
 */
Camera::Camera() {

    m_position = {0, 0, 0};
}

void Camera::init(int *screen_width, int *screen_height, double field_of_view) {

    m_screen_width = screen_width;
    m_screen_height = screen_height;

    m_field_of_view = field_of_view;
    m_dist_to_draw_plane = (*screen_width / 2) / tan(glm::radians(m_field_of_view / 2));
    m_draw_radius = calculate_draw_radius();
    m_rotation_speed = 0.5;

    to_start_position();
}

/*
 *   <==========get methods=========>
 */
glm::vec2 Camera::get_screen_center() {
    return glm::vec2(*m_screen_width / 2, *m_screen_height / 2);
}

unsigned long Camera::get_distance_to_camera(glm::vec3 object_position) {
    return length((m_position - object_position));
}

/*
 * <=========set methods=========>
 */
void Camera::set_orientation(glm::vec3 new_orientation) {
    glm::vec3 diff;

    glm::vec3 buf = glm::cross(m_orientation, m_plane_vec);
    new_orientation = glm::normalize(new_orientation);
    glm::vec3 buf2  = glm::cross(new_orientation, buf);
    m_plane_vec = -glm::normalize(glm::cross(new_orientation, buf2)) * m_dist_to_draw_plane;
    m_orientation = new_orientation * m_dist_to_draw_plane;
}

void Camera::set_position(glm::vec3 new_position)
{
    m_position = new_position;
}

void Camera::set_rotation_speed(float new_speed)
{
    m_rotation_speed = new_speed;
}

void Camera::set_draw_radius(float new_radius)
{
    m_draw_radius = new_radius;
}
/*
 *   <==========change state methods=========>
 */
void Camera::move(glm::vec3 move_vector) {
    m_position += move_vector;
}
void Camera::rotate(glm::quat rot_quat) {
    m_orientation = glm::normalize((m_orientation)*rot_quat) * m_dist_to_draw_plane;
    m_plane_vec = glm::normalize((m_plane_vec)*rot_quat) * m_dist_to_draw_plane;
}

void Camera::to_start_position() {

    m_position = glm::vec3(0, 0, -80000000 * (m_dist_to_draw_plane));
    m_orientation = glm::vec3(0, 0, 1) * m_dist_to_draw_plane;
    m_plane_vec = glm::vec3(0, 1, 0) * m_dist_to_draw_plane;
    ;
}

/***
 *<==========draw calculation methods=========>
 */
float Camera::calculate_size_on_plane(glm::vec3 object_position, glm::vec3 vector_to_plane_pos) {
    glm::vec3 camera_to_object_vector = m_position - object_position;
    return glm::length(camera_to_object_vector) / glm::length(vector_to_plane_pos);
}

float Camera::calculate_draw_radius() {
    return sqrt(pow(m_dist_to_draw_plane, 2) +
                pow(sqrt(pow(*m_screen_width, 2) + pow(*m_screen_height, 2)), 2));
}

glm::vec4 Camera::calculate_position_on_draw_plane(glm::vec3 object_position) {
    // calculate distance for object planet
    glm::vec3 camera_to_object_vector = object_position - m_position;
    double winkel_rad =
        acos(glm::dot(glm::normalize(m_orientation), glm::normalize(camera_to_object_vector)));

    float f = m_dist_to_draw_plane / cos(winkel_rad);

    glm::vec3 from_direction_to_plane_pos =
        ((glm::normalize(camera_to_object_vector) * f) - m_orientation) + glm::vec3(450, 450, 0);

    return glm::vec4(from_direction_to_plane_pos + m_position, winkel_rad);
}

glm::quat Camera::calculate_rotation_quat_from_drag(glm::vec2 mouse_rel) {
    float x_angle = -glm::radians((float)mouse_rel.y);
    float y_angle = glm::radians((float)mouse_rel.x);

    glm::vec3 camera_z_axis = glm::normalize(m_plane_vec);
    glm::vec3 camera_x_axis = glm::normalize(glm::cross(m_plane_vec, m_orientation));

    glm::quat x_rot = glm::quat(cos(y_angle / 2), sin(y_angle / 2) * camera_z_axis);
    glm::quat y_rot = glm::quat(cos(x_angle / 2), sin(x_angle / 2) * camera_x_axis);

    return (x_rot * y_rot);
}
