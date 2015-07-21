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
    number_of_particles = 0;

}
void Particle::remove(unsigned long vector_index)
{
    velocity_vectors.erase(velocity_vectors.begin() + vector_index);
    positions.erase(positions.begin() + vector_index);
    masses.erase(masses.begin() + vector_index);
    radiuses.erase(radiuses.begin() + vector_index);
    number_of_particles--;
}

double Particle::get_distance_to_center(unsigned long particle_index)
{
    return positions[particle_index].getLength();
}

// move each object by their velocity, if the stepsize is equal to 1second 
void Particle::move_Object(unsigned long particle_index, double step_size = 0.00000001)
{
    //positions[particle_index].display();
    positions[particle_index] += velocity_vectors[particle_index]/step_size;
    //positions[particle_index].display();

}

double Particle::getDistanceOfTwoObjects(unsigned long ob1, unsigned long ob2)
{
    return Vec3<double>::getDistance(positions[ob1], positions[ob2]);
}

Vec3<double> Particle::getVelocityVector(unsigned long particle_index)
{
    return velocity_vectors[particle_index];	
}

void Particle::addAccelerationVector(unsigned long particle_index, Vec3<double> accelerationVector)
{
    velocity_vectors[particle_index] += accelerationVector;
}

Vec3<double> Particle::getPostion(unsigned long particle_index)
{
    return positions[particle_index];
}

double Particle::getMass(unsigned long particle_index)
{
    return masses[particle_index];
}

double Particle::getRadius(unsigned long particle_index)
{
    return radiuses[particle_index];
}

unsigned long Particle::getNumberOfParticles()
{
    return number_of_particles;
}

unsigned long Particle::generateRandomParticle(double max_pos, double max_velo,
        double max_mass, double max_radius)
{
    Vec3<double> new_position = generateRandomVec3<double>(-max_pos,max_pos);
    Vec3<double> new_velocity_vector = generateRandomVec3<double>(0,max_velo);
    double new_mass = generateRandomNumber<double>(0,max_mass);
    double new_radius = generateRandomNumber<double>(0,max_radius);

    return createParticle(
            new_position,
            new_velocity_vector,
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

    return createParticle(
            new_position,
            new_velocity_vector,
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
    velocity_vectors.push_back(v);
    positions.push_back(p);
    masses.push_back(m);
    radiuses.push_back(r);
    number_of_particles++;
    return number_of_particles - 1;
}


void Particle::printParticle(unsigned long particle_index)
{
    printf("ParicleID: %lu\nDistance from center: %f\nPosition:\n	X: %f\n	Y: %f\n	Z: %f\nVelocity: %f\n	X: %f\n	Y: %f\n	Z: %f\nMass: %f\nRadius %f\n\n",
            particle_index,
            positions[particle_index].getLength(),
            positions[particle_index].getX(),
            positions[particle_index].getY(),
            positions[particle_index].getZ(),
            velocity_vectors[particle_index].getLength(),
            velocity_vectors[particle_index].getX(),
            velocity_vectors[particle_index].getY(),
            velocity_vectors[particle_index].getZ(),
            masses[particle_index],
            radiuses[particle_index]);
}

void Particle::printAllParticles()
{
    for(unsigned long i = 0; i <  number_of_particles; i++)
    {
        printParticle(i);
    }

}

void Particle::writePositionToFile(std::string filename)
{
    std::ofstream file(filename, std::ofstream::app);
    if(file.is_open())
    {

        for(unsigned long i = 0; i < number_of_particles; i++)
        {
            file << positions[i].toString() << "\n";
        }
        file << ">\n";
    }
}



//TODO: Save funktion die die daten aus Particle in eine datei schreibt
