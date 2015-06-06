#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include <vector>
#include <math.h>
#include <iostream>
#include <ctime>
#include <utility> 
#include <cstdlib>
#include "random_generator.hpp"

// center object has to be at <0.0.0>

class Particle
{
private:
	std::vector<Vec3<double> > velocity_vectors;
	std::vector<Vec3<double> > positions;
	std::vector<double> masses;
	std::vector<double> radiuses;
	unsigned long number_of_particles;

public:
	void remove(unsigned long vector_index);

	double get_distance_to_center(unsigned long particle_index);

	/**
		move each object by their velocity, if the stepsize is equal to 1second 
	*/
	void move_Object(unsigned long particle_index,double stepsize);
	
	double getDistanceOfTwoObjects(unsigned long ob1, unsigned long ob2);

	Vec3<double> getVelocityVector(unsigned long particle_index);


	Vec3<double> getPostion(unsigned long particle_index);

	double getMass(unsigned long particle_index);

	double getRadius(unsigned long particle_index);

	unsigned long getNumberOfParticles();

	/**
		Discription:
			Generates a new particle witch random atributes \n
		Parameters:

		Return value:
			returns the index of the new Particle
	*/
	unsigned long generateRandomParticle(
		std::pair<double,double> range_position,
		std::pair<double,double> range_velocity,
		std::pair<double,double> range_mass,
		std::pair<double,double> range_radius);
	/**
		Discription:
			Generates a new Particle with specified values\n
		\n
		Parameters:\n
			v: velocity vector for the new particle 	Vec3<double>\n
			p: position vector for the new particle 	Vec3<double>\n
			m: mass for the new object 					double\n
			r: radius for the new Object 				double\n
		\n
		Return value:\n
			returns the index of the new Particle\n
	*/
	unsigned long createParticle(
		Vec3<double> p,
		Vec3<double> v,
		double m,
		double r );

	void printParticle(unsigned long particle_index);

	void printAllParticles();

	//TODO: Save funktion die die daten aus Particle in eine datei schreibt
};

#endif