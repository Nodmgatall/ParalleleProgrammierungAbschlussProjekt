#include "camera.hpp"
#include <string>
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
Camera::Camera()
{
    x = 0;
    y = 0;
    z = 0;

    x_axis = {1,0,0};
    y_axis = {0,1,0};
    z_axis = {0,0,1};
}

void Camera::init(int screen_width, int screen_height, double field_of_view)
{
    m_field_of_view = field_of_view;
    m_screen_width = screen_width;
    m_screen_height = screen_height;
    x = screen_width/2;
    y = screen_height/2;
    
    m_dist_to_draw_plane = (screen_width/2) / tan(m_field_of_view/2); 
    m_orientation = glm::vec3(0,0,-1) * m_dist_to_draw_plane;
    z = 1000; 
    m_position = glm::vec3(x,y,z);
}

void Camera::move(glm::vec3 move_vector)
{
    x+= move_vector.x;
    y+= move_vector.y;
    z+= move_vector.z;
}

void Camera::rotate(double new_angle)
{
    m_x_rot_deg += new_angle;
    float angle = m_x_rot_deg;
    float nx = m_orientation.x * cos(angle) - m_orientation.y * sin(angle);
    float ny = m_orientation.x * sin(angle) - m_orientation.y * cos(angle);
    m_orientation.y = nx;
    m_orientation.z = ny;
    std::cout << glm::to_string(m_orientation) << std::endl;
}

unsigned long Camera::get_distance_to_camera(glm::vec3 object_position)
{
    return length((glm::vec3(x,y,z) - object_position));
}

double Camera::get_size_on_plane(glm::vec3 object_position, glm::vec3 vector_to_plane_pos)
{
    glm::vec3 camera_position = {x,y,z};
    glm::vec3 camera_to_object_vector = camera_position - object_position;
    return glm::length(camera_to_object_vector) / glm::length(vector_to_plane_pos);  
}

glm::vec4 Camera::calculate_position_on_draw_plane(glm::vec3 object_position)
{
    //calculate distance for object planei
    glm::vec3 camera_to_object_vector = m_position - object_position;
    glm::vec3 distance_from_center_of_plane_on_plane; 
    double winkel_rad = acos(glm::dot(m_orientation,camera_to_object_vector) /
            (glm::length(camera_to_object_vector) * glm::length(m_orientation)));
    float f = tan(winkel_rad) * glm::length(m_orientation);
    return glm::vec4(m_position,0) + glm::vec4(glm::normalize(camera_to_object_vector) * f,winkel_rad); 
}
