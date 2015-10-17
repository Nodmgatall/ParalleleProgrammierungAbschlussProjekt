#include "camera.hpp"
#include <string>
#include <math.h>
Camera::Camera()
{
    x = 0;
    y = 0;
    z = 5000000000;
}

void Camera::init(int screen_width, int screen_height, double field_of_view)
{
    m_field_of_view = field_of_view;
    m_screen_width = screen_width;
    m_screen_height = screen_height;
    m_dist_to_draw_plane = (screen_width/2) / tan(m_field_of_view); 
    m_orientation = Vec3<double>(0,0,-1) * m_dist_to_draw_plane;
}

void Camera::move(Vec3<double> move_vector)
{
    x+= move_vector.getX();
    y+= move_vector.getY();
    z+= move_vector.getZ();
}

void Camera::rotate_x(double angle)
{
    std::cout << angle << std::endl; 
}

unsigned long Camera::get_distance_to_camera(Vec3<double> object_position)
{
    return (Vec3<double>(0,0,z-object_position.getZ())).getLength();
}

Vec3<double> Camera::calculate_position_on_draw_plane(Vec3<double> object_position)
{
    //calculate distance for object planei
    Vec3<double> camera_position = Vec3<double>(x,y,z);
    Vec3<double> view_direction_vector = camera_position - object_position;
    Vec3<double> distance_from_center_of_plane_on_plane = 
        (m_orientation) * 
        tan(
                acos((
                        view_direction_vector.dotProduct(object_position)
                        /
                        (
                         view_direction_vector.getLength() * object_position.getLength()
                        )
                     ))
           );  

    return camera_position + m_orientation + distance_from_center_of_plane_on_plane; 



}
