#include "Util/random_generator.hpp"
#include "particle.hpp"
#include "physics.hpp"
#ifdef PARALLEL_BUILD
#include "globals.h"
#endif

#include <string>
#include <vector>
#include <math.h>
#include <iostream>
#include <ctime>
#include <utility> 
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <cmath>

//#include <boost/serialization/vector.hpp>


//#include <boost/archive/binary_oarchive.hpp>
//#include <boost/archive/binary_iarchive.hpp>
//#include <boost/archive/text_oarchive.hpp>
//#include <boost/archive/text_iarchive.hpp>



const double PI = 3.14159265359;
//TODO SWITCH TO UINTX_T
// center object has to be at <0.0.0>

Particle::Particle()
{
    m_number_of_particles = 0;
    m_max_id = 0;
    m_dt = 0.00024;
    m_time_simulated = 0;

}
void Particle::remove(unsigned long vector_index)
{
    m_velocity_vectors.erase(m_velocity_vectors.begin() + vector_index);
    m_positions.erase(m_positions.begin() + vector_index);
    m_masses.erase(m_masses.begin() + vector_index);
    m_radiuses.erase(m_radiuses.begin() + vector_index);
    m_ids.erase(m_ids.begin() + vector_index);
    m_number_of_particles--;
    return;
}

double Particle::get_distance_to_center(unsigned long particle_index)
{
    return m_positions[particle_index].getLength();
}

double Particle::get_time_simulated()
{
    return m_time_simulated;
}

std::vector<Vec3<double>> Particle::get_velo_vector()
{
    return m_velocity_vectors;
}

std::vector<Vec3<double>> &Particle::get_pos_vector()
{
    return m_positions;
}

std::vector<double> &Particle::get_mass_vector()
{
    return m_masses;
}

std::vector<double> &Particle::get_radius_vector()
{
    return m_radiuses;
}

std::vector<unsigned long> &Particle::get_id_vector()
{
    return m_ids;
}

void Particle::update_velo_vector(std::vector<Vec3<double>> new_velo_vector)
{
    m_velocity_vectors = new_velo_vector;
}

void Particle::update_pos_vector(std::vector<Vec3<double>> new_pos_vector)
{
    m_velocity_vectors = new_pos_vector;
}


double Particle::get_max_velo()
{
    return m_max_velo;
}

void Particle::set_max_velo(double new_max_velo)
{
    m_max_velo = new_max_velo;
}

void Particle::apply_gravity(unsigned long start_idx, unsigned long end_idx)
{
    if(end_idx == 0)
    {
        end_idx = m_positions.size();   
    }

    for (unsigned long particle_index = start_idx; particle_index < end_idx; particle_index++)
    {
        applyGravity(this,particle_index,m_dt);
    }

}

void Particle::move_objects(unsigned long start_idx, unsigned long end_idx)
{
    if(end_idx == 0)
    {
        end_idx = m_positions.size();   
    }

    for(unsigned long idx = start_idx; idx <= end_idx; idx++)
    {
        move_Object(idx);
    }
}

// move each object by their velocity, if the stepsize is equal to 1second 
void Particle::move_Object(unsigned long particle_index)
{
    //m_positions[particle_index].display();
    m_positions[particle_index] += m_velocity_vectors[particle_index] / m_dt;
    //m_positions[particle_index].display();

}

Vec3<double> Particle::getVelocityVector(unsigned long particle_index)
{
    return m_velocity_vectors[particle_index];	
}

void Particle::addAccelerationVector(unsigned long particle_index, Vec3<double> accelerationVector)
{
    m_velocity_vectors[particle_index] += accelerationVector;
}

Vec3<double> Particle::getPosition(unsigned long particle_index)
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
    return m_number_of_particles;
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

    new_position.setZ(0);
    new_velocity_vector.setZ(0);
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
    m_ids.push_back(m_max_id);
    m_number_of_particles++;
    m_max_id++;
    return m_number_of_particles - 1;
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
    for(unsigned long i = 0; i <  m_number_of_particles; i++)
    {
        printParticle(i);
    }
}

void Particle::write_to_file(std::string filename, unsigned long iteration_number, std::ios_base::openmode mode)
{
    std::ofstream file(filename, mode);
    if(file.is_open())
    {
        std::string deleted_ids_string;

        file << std::to_string(iteration_number) + " " + std::to_string(m_number_of_particles) + " " + std::to_string(m_dt) + "\n"; 
        if(m_deleted_ids_in_iteration.size() > 0)
        {
            deleted_ids_string = std::to_string(m_deleted_ids_in_iteration[0]);
            for(unsigned long i = 1; i < m_number_of_particles; i++)
            {
                deleted_ids_string += " ";
                deleted_ids_string += std::to_string(m_deleted_ids_in_iteration[i]);
            }
        }
        else
        {
            deleted_ids_string = "NO_COLLISIONS";
        }

        file << deleted_ids_string + "\n";

        for(unsigned long i = 0; i < m_number_of_particles; i++)
        {
            file << std::to_string(m_ids[i]) + " " + m_positions[i].toString() + m_velocity_vectors[i].toString() + std::to_string(m_masses[i]) + " " + std::to_string(m_radiuses[i]) + "\n";
        }
        file << ">\n";
    }
}

void Particle::merge_objects(unsigned long obj_id_1, unsigned long obj_id_2)
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

    Vec3<double> new_velocity = m_velocity_vectors[obj_id_2];
    double new_mass = m_masses[obj_id_1] + m_masses[obj_id_2];
    double new_volume = (4/3) * pow(m_radiuses[obj_id_1],3) + (4/3) * pow(m_radiuses[obj_id_2],3);
    Vec3<double> dist_normalized = (m_positions[obj_id_1] - m_positions[obj_id_2]);
    dist_normalized.normalise();
    //Vec3<double> new_position = m_positions[obj_id_2] + (dist_normalized * m_radiuses[obj_id_2]);
    double new_radius = pow((new_volume * 3) / (PI * 4),(-3/2));
    if(m_masses[obj_id_1] > m_masses[obj_id_2])
    {
        new_velocity = m_velocity_vectors[obj_id_1];
    }

    m_masses[obj_id_2] = new_mass;
    m_velocity_vectors[obj_id_2] = new_velocity;
    m_radiuses[obj_id_2] = new_radius;
    std::cout << m_max_id - 1 << std::endl;
    remove(obj_id_1);
    return;
}

void Particle::load_data_from_file(std::string filepath, unsigned long &number_of_previous_iterations)
{
    std::ifstream file(filepath);
    std::string s;
    std::string num_objects;
    std::string num_iterations;
    if(file.is_open())
    {
        getline(file,s);
        std::stringstream ss(s);
        getline(ss, num_iterations, ' ');
        number_of_previous_iterations = stoul(num_iterations,NULL,0);
        getline(ss, num_objects,' ');
        getline(file,s);
        while(getline(file,s))
        {
            if (s == ">")
            {
                file.close();
                return;
            }
            else 
            {
                //std::cout << s << std::endl;
                std::string id, x, y, z, velo_x, velo_y, velo_z, mass, radius;
                ss = std::stringstream(s);
                getline(ss,id,' ');
                getline(ss,x,' ');
                getline(ss,y,' ');
                getline(ss,z,' ');
                getline(ss,velo_x,' ');
                getline(ss,velo_y,' ');
                getline(ss,velo_z,' ');
                getline(ss,mass,' ');
                getline(ss,radius,' ');

                m_ids.push_back(std::strtoul(id.c_str(),NULL,0));
                m_positions.push_back(Vec3<double>(std::stod(x),std::stod(y),std::stod(z)));
                m_velocity_vectors.push_back(Vec3<double>(std::stod(velo_x),std::stod(velo_y), std::stod(velo_z)));
                m_masses.push_back(std::stod(mass));
                m_radiuses.push_back(std::stod(radius));
                //std::cout << m_positions.back().toString() << std::endl;
                m_number_of_particles++;

            }
        }
    }
    else
    {
        std::cout << filepath <<" could not be opened" << std::endl;
        exit(EXIT_FAILURE);
    }
}

bool Particle::check_for_collision(unsigned long index_1, unsigned long index_2, double time_of_closest_approach)
{
    //toca = time_of_closest_approach
    Vec3<double> pos1_at_toca = m_positions[index_1] + (m_velocity_vectors[index_1] * time_of_closest_approach);
    Vec3<double> pos2_at_toca = m_positions[index_2] + (m_velocity_vectors[index_2] * time_of_closest_approach);
    double dist =  (pos2_at_toca - pos1_at_toca).getLength();

    if(dist < m_radiuses[index_2] + m_radiuses[index_1])
    {
        return true;
    } 
    return false;
}
    template <typename Type>
void swap(std::vector<Type> &vec, unsigned long idx1, unsigned long idx2)
{
    Type buffer = vec[idx1];
    vec[idx1] = vec[idx2];
    vec[idx2] = buffer;
    return;

}

void Particle::particle_bubble_sort(unsigned long idx_start, unsigned long idx_end)
{
    if(idx_end == 0)
    {
        idx_end = m_positions.size();   
    }
    bool all_done = false;
    while(!all_done)
    {
        all_done = true;
        for(unsigned long i = idx_start + 1; i < idx_end; i++)
        {
            if(m_positions[i].getLength() < m_positions[i-1].getLength())
            {
                all_done = false;
                //std::cout << "swaped: " << i << " " <<  i-1 << std::endl; 
                swap<Vec3<double>>(m_positions, i, i-1);
                swap<Vec3<double>>(m_velocity_vectors, i, i-1);
                swap<double>(m_masses, i, i-1);
                swap<double>(m_radiuses, i, i-1);
                swap<unsigned long>(m_ids, i, i - 1);
            }
        }
    }
    //std::cout << "runs: " << runs << " " << "swaps: " << swaps << std::endl; 
    //std::cout << "xswaps: "<< xswaps << std::endl; 
}

double Particle::get_dt()
{
    return m_dt;
}


bool Particle::limit(unsigned long index_1, unsigned long index_2)
{
    double dist = fabs(m_positions[index_1].getLength() - m_positions[index_2].getLength());

    return dist > m_radiuses[index_1] + m_radiuses[index_2] + m_max_velo * 2;
}

void Particle::detect_collision(unsigned long index_1, unsigned long index_2)
{
    if(index_2 == 0)
    {
        index_2 = m_positions.size();
    }
    unsigned long number_of_collisions = 0;
    unsigned long j; 
    double time_of_closest_approach;
    for(unsigned long i = index_1; i < index_2 - number_of_collisions; i++)
    {
        j = i;
        while( j > 0 && !limit(i, j-1))
        {

            time_of_closest_approach = get_time_of_closest_approach(m_positions[i], m_velocity_vectors[i]/m_dt, m_positions[j - 1], m_velocity_vectors[j - 1]/m_dt);

            if(check_for_collision(i, j -1, time_of_closest_approach))
            {
                if(j - 1 != 0) std::cout << "Collision Detected: " << m_ids[i] << " " << m_ids[j - 1] << std::endl;
                merge_objects(i,j-1);
                number_of_collisions++;
                i--;
            }            
            j--;
        }
    }

    m_time_simulated += 1;
    //std::cout << "1/m_dt: "<< 1/m_dt << std::endl;
}



std::pair<double,double> Particle::calculate_possible_collison_point(unsigned long index_1, unsigned long index_2)
{

    //TDDO CHECK FOR PARALLEL VECTORS !!!
    double lamda;
    double sigma;

    double x1 = m_positions[index_1].getX();
    double y1 = m_positions[index_1].getY();
    double z1 = m_positions[index_1].getZ();

    double x2 = m_positions[index_2].getX();
    double y2 = m_positions[index_2].getY();
    double z2 = m_positions[index_2].getZ();

    double vx1 = m_velocity_vectors[index_1].getX();
    double vy1 = m_velocity_vectors[index_1].getY();
    double vz1 = m_velocity_vectors[index_1].getZ();

    double vx2 = m_velocity_vectors[index_2].getX();
    double vy2 = m_velocity_vectors[index_2].getY();
    double vz2 = m_velocity_vectors[index_2].getZ();

    lamda = ( ( (y1 - y2)/ vy2)-( (x1 + -x2 )/ vx2) ) / ( ( (vx1 * vy2) - (vy1* vx2) ) / (vx2 * vy2));
    sigma = (x1 - x2 + (vx1 * lamda)) / vx2;


    if(z1 + vz1 * lamda == z2 + vz2 * sigma)
    {

    }
    return std::pair<double,double>(lamda,sigma);
}

/*
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
    */

    //void Particle::write_to_archive(boost::archive::binary_oarchive *oa)
    //{
    //    *oa << m_positions;
    //    *oa << m_velocity_vectors;
    //    *oa << m_masses;
    //    *oa << m_radiuses;
    //    *oa << m_ids;
    //    //oa << m_deleted_ids_in_iteration;
    //    *oa << m_dt;
    //}

