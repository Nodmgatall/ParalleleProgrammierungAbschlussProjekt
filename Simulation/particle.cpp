#include "Util/random_generator.hpp"
#include "particle.hpp"

#include <string>
#include <vector>
#include <math.h>
#include <iostream>
#include <ctime>
#include <utility> 
#include <cstdlib>
#include <fstream>



//TODO SWITCH TO UINTX_T
// center object has to be at <0.0.0>

Particle::Particle()
{
    m_number_of_particels = 0;
    m_max_id = 0;

}
void Particle::remove(unsigned long vector_index)
{
    m_velocity_vectors.erase(m_velocity_vectors.begin() + vector_index);
    m_positions.erase(m_positions.begin() + vector_index);
    m_masses.erase(m_masses.begin() + vector_index);
    m_radiuses.erase(m_radiuses.begin() + vector_index);
    m_used_ids.erase(m_used_ids.begin() + vector_index);
    m_number_of_particels--;
    return;
}

void Particle::remove_by_id(unsigned long particle_id)
{
    unsigned long vector_index = get_vector_index(particle_id);
    remove(vector_index);
    return;
}


unsigned long Particle::get_vector_index(unsigned long id)
{
    unsigned long sub_vector_size;
    unsigned long index;
    unsigned long sub_vector_end = id;

    if(id > m_used_ids.size())
    {
        sub_vector_end = m_used_ids.size();
    }

    if(m_used_ids[sub_vector_end] == id)
    {
        std::cout << "wanted: "<< id << "    simple deleted: "<< m_used_ids[id] << std::endl;
        //m_used_ids.erase(m_used_ids.begin() + sub_vector_end);
        return sub_vector_end;
    }

    sub_vector_size = id / 2;
    index = 0;
    while(index < sub_vector_end)
    {
        //std::cout << "sub_vector_size "<< sub_vector_size << std::endl;
        for(int i = 0; i < 10 ; i ++)
        {
            int rofl;
            int lol = 0;
            lol = i;
        
            rofl = lol;
            lol = rofl;
        }
        unsigned long current_index = index + sub_vector_size; 
        if(m_used_ids[current_index] == id)
        {
            std::cout << "wanted: " << id << "   Deleted: " << m_used_ids[current_index] << std::endl;
            //m_used_ids.erase(m_used_ids.begin() + current_index);
            return current_index; 
        }
        else
        {
            if(m_used_ids[current_index] < id)
            {
                //std::cout << "current_index > id" << std::endl << m_used_ids[current_index] << std::endl << id << std::endl;
                index = current_index + 1;
            }
            else
            {
                //std::cout << "current_index < id" << std::endl << m_used_ids[current_index] << std::endl << id << std::endl;
                sub_vector_end = current_index;   
            }
            sub_vector_size = sub_vector_size / 2;
        }
    }
    std::cout << "end" << std::endl;
    return m_max_id + 1;
}


double Particle::get_distance_to_center(unsigned long particle_index)
{
    return m_positions[particle_index].getLength();
}

// move each object by their velocity, if the stepsize is equal to 1second 
void Particle::move_Object(unsigned long particle_index, double step_size = 0.00000001)
{
    //m_positions[particle_index].display();
    m_positions[particle_index] += m_velocity_vectors[particle_index]/step_size;
    //m_positions[particle_index].display();

}

double Particle::getDistanceOfTwoObjects(unsigned long ob1, unsigned long ob2)
{
    return Vec3<double>::getDistance(m_positions[ob1], m_positions[ob2]);
}

Vec3<double> Particle::getVelocityVector(unsigned long particle_index)
{
    return m_velocity_vectors[particle_index];	
}

void Particle::addAccelerationVector(unsigned long particle_index, Vec3<double> accelerationVector)
{
    m_velocity_vectors[particle_index] += accelerationVector;
}

Vec3<double> Particle::getPostion(unsigned long particle_index)
{
    return m_positions[particle_index];
}

double Particle::getMass(unsigned long particle_index)
{
    return m_masses[particle_index];
}

double Particle::getRadius(unsigned long particle_index)
{
    return m_radiuses[particle_index];
}

unsigned long Particle::getNumberOfParticles()
{
    return m_number_of_particels;
}

Vec3<double> Particle::calculate_ortogonal_vector_to_pos_vec(Vec3<double> pos_vec, Vec3<double> velocity_vector)
{
    //Step1: calculate a vector v1 that in combination with pos_vec describes a plane from which
    //we can calculate a vector v2 that is in an 90 deg angle to pos vec and is kon the plane that
    //is described by pos_vec and (0,0,1)
    Vec3<double> ort_vec = Vec3<double>::crossProduct(pos_vec, Vec3<double>(0,0,1));
    //Step2: calculate v2 
    Vec3<double>v1 = Vec3<double>::crossProduct(ort_vec,pos_vec);
    //Step3: now we calculate the vector that hat a 90 deg angle to pos_vec
    //and has a 90 deg angele to the plane described by pos_vec and v2
    Vec3<double> v2 = Vec3<double>::crossProduct(v1,pos_vec);
    //Step4: Normalise v2 
    v2.normalise();
    //Step4: Give v2 the wanted length and so the wanted velocity
    v2 *= velocity_vector.getLength();
    return v2;
}


unsigned long Particle::generateRandomParticle(double max_pos, double max_velo,
        double max_mass, double max_radius)
{
    Vec3<double> new_position = generateRandomVec3<double>(-max_pos,max_pos);
    Vec3<double> new_velocity_vector = generateRandomVec3<double>(0,max_velo);
    double new_mass = generateRandomNumber<double>(0,max_mass);
    double new_radius = generateRandomNumber<double>(0,max_radius);

    //new_position.setZ(0);
    //new_velocity_vector.setZ(0);
    return createParticle(
            new_position,
            calculate_ortogonal_vector_to_pos_vec(new_position,new_velocity_vector),
            new_mass,
            new_radius);

}

unsigned long Particle::generateRandomParticle(
        std::pair<double,double> range_position,
        std::pair<double,double> range_velocity,
        std::pair<double,double> range_mass,
        std::pair<double,double> range_radius)
{
    Vec3<double> new_position = generateRandomVec3<double>(
            range_position.first,
            range_position.second);
    Vec3<double> new_velocity_vector =  generateRandomVec3<double>(
            range_velocity.first,
            range_velocity.second);
    double new_mass = generateRandomNumber<double>(
            range_mass.first,
            range_mass.second);
    double new_radius = generateRandomNumber<double>(
            range_radius.first,
            range_radius.second);

    //new_position.setZ(0);
    //new_velocity_vector.setZ(0);
    return createParticle(
            new_position,
            calculate_ortogonal_vector_to_pos_vec(new_position,new_velocity_vector),
            new_mass,
            new_radius);

}

//IDEA : retutn iterator instead of index
unsigned long Particle::createParticle(
        Vec3<double> p,
        Vec3<double> v,
        double m,
        double r )
{
    m_velocity_vectors.push_back(v);
    m_positions.push_back(p);
    m_masses.push_back(m);
    m_radiuses.push_back(r);
    m_used_ids.push_back(m_max_id);
    m_number_of_particels++;
    m_max_id++;
    return m_number_of_particels - 1;
}


void Particle::printParticle(unsigned long particle_index)
{
    printf("ParicleID: %lu\nDistance from center: %f\nPosition:\n	X: %f\n	Y: %f\n	Z: %f\nVelocity: %f\n	X: %f\n	Y: %f\n	Z: %f\nMass: %f\nRadius %f\n\n",
            particle_index,
            m_positions[particle_index].getLength(),
            m_positions[particle_index].getX(),
            m_positions[particle_index].getY(),
            m_positions[particle_index].getZ(),
            m_velocity_vectors[particle_index].getLength(),
            m_velocity_vectors[particle_index].getX(),
            m_velocity_vectors[particle_index].getY(),
            m_velocity_vectors[particle_index].getZ(),
            m_masses[particle_index],
            m_radiuses[particle_index]);
}

void Particle::printAllParticles()
{
    for(unsigned long i = 0; i <  m_number_of_particels; i++)
    {
        printParticle(i);
    }

}

void Particle::writePositionToFile(std::string filename)
{
    std::ofstream file(filename, std::ofstream::app);
    if(file.is_open())
    {

        for(unsigned long i = 0; i < m_number_of_particels; i++)
        {
            file << m_positions[i].toString() + m_velocity_vectors[i].toString() + std::to_string(m_masses[i]) + " " + std::to_string(m_radiuses[i]) + "\n";
        }
        file << ">\n";
    }
}

void Particle::calculate_collision(unsigned long obj_id_1, unsigned long obj_id_2)
{
    if(obj_id_1 == 0)
    {
        remove(obj_id_2);
        return;
    }
    if(obj_id_2 == 0)
    {
        remove(obj_id_1);
        return;
    }
    double impulse_obj_1 = (m_velocity_vectors[obj_id_1] * m_masses[obj_id_1]).getLength();
    double impulse_obj_2 = (m_velocity_vectors[obj_id_2] * m_masses[obj_id_2]).getLength();
    //To remove "not used error"
    impulse_obj_1 = impulse_obj_2;
    impulse_obj_2 = impulse_obj_1;
}

void Particle::merge_objects(unsigned long obj_id_1, unsigned long obj_id_2)
{
    remove(obj_id_1);
    remove(obj_id_2);
    return;
}
