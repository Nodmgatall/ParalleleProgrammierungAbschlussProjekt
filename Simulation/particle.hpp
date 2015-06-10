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
/**
	Data structure to store and manage particles.\n
	Paritcles are stored in a std::vector<double> and are accsessed by
	their index.
*/
class Particle
{
private:
	std::vector<Vec3<double> > velocity_vectors;
	std::vector<Vec3<double> > positions;
	std::vector<double> masses;
	std::vector<double> radiuses;
	unsigned long number_of_particles;

public:

	Particle();
		/** removes single particle */
	void remove(unsigned long vector_index);

	/** returns the distance from the center of the system */
	double get_distance_to_center(unsigned long particle_index);

	/**
		move each object by their velocity, if the stepsize is equal to 1second 
	*/
	void move_Object(unsigned long particle_index,double stepsize);
	
	/** returns the distance between two  particles */
	double getDistanceOfTwoObjects(unsigned long ob1, unsigned long ob2);

	/** returns the velocity vector of given particle */
	Vec3<double> getVelocityVector(unsigned long particle_index);

	/** returns the positon of given particle */
	Vec3<double> getPostion(unsigned long particle_index);

	//TODO:
	/** returns a pointer to a copied positions Vector for saving */ 
	Vec3<Vec3<double> > savePositionData();

	void writePositionToFile(char* filename);

	/** returns the mass of given particle */
	double getMass(unsigned long particle_index);

	/** returns the radius of given particle */
	double getRadius(unsigned long particle_index);

	/** returns the number of existing particles */
	unsigned long getNumberOfParticles();

	/**
		Discription:\n
			Generates a new particle witch random atributes \n
		Return value:\n
			returns the index of the new Particle
	*/
	unsigned long generateRandomParticle(
		std::pair<double,double> range_position,
		std::pair<double,double> range_velocity,
		std::pair<double,double> range_mass,
		std::pair<double,double> range_radius);

	/**
		Discription:\n
			Generates a new Particle with specified values\n
		Return value:\n
			returns the index of the new Particle
	*/
	unsigned long createParticle(
		Vec3<double> positon,
		Vec3<double> velocity_vector,
		double mass,
		double radius );

	/** prints all data about given particle to the console 
		Example output:\n
		ParicleID: 0\n
		Distance from center: 58984654.079628\n
		Position:\n
		        X: 2656945.474053\n
		        Y: 46291973.436072\n
		        Z: 36457965.563581\n
		Velocity: 1.805316\n
		        X: 0.730751\n
		        Y: 1.311961\n
		        Z: 1.001961\n
		Mass: 2255.501134\n
		Radius 429.489498\n

	*/
	void printParticle(unsigned long particle_index);

	/** prints all particles to the console */
	void printAllParticles();

	//TODO: Save funktion die die daten aus Particle in eine datei schreibt
};


#endif