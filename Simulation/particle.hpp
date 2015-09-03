#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include "Util/random_generator.hpp"

#include <string>
#include <vector>
#include <math.h>
#include <iostream>
#include <ctime>
#include <utility> 
#include <cstdlib>
#include <map>

// boost's serialization stuff for vectors
#include <boost/serialization/vector.hpp>

// boost's archive stuff for serialization
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

// center object has to be at <0.0.0>
/**
  Data structure to store and manage particles.\n
  Paritcles are stored in a std::vector<double> and are accsessed by
  their index.
  */
class Particle
{
    private:
        // members needed for serialization
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version);

        // 'regular' class members
        std::vector<Vec3<double> > m_velocity_vectors;
        std::vector<Vec3<double> > m_positions;
        std::vector<double> m_masses;
        std::vector<double> m_radiuses;
        std::vector<unsigned long> m_ids;
        unsigned long m_number_of_particles;
        unsigned long m_max_id;
        std::vector<unsigned long> m_deleted_ids_in_iteration;

    public:

        Particle();
        /** removes single particle */
        void remove(unsigned long vector_index);

    //void remove_by_id(unsigned long particle_id);

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

        /** adds a acceleration vector to the velocity vectof of specified object*/
        void addAccelerationVector(unsigned long particle_index, Vec3<double> accelerationVector);

        /** returns the positon of given particle */
        Vec3<double> getPosition(unsigned long particle_index);

        //TODO:
        /** returns a pointer to a copied m_positions Vector for saving */ 
        Vec3<Vec3<double> > savePositionData();

        void write_to_file(std::string filename, unsigned long iteration_number, std::ios_base::openmode mode) ;
        /** returns the mass of given particle */
        double getMass(unsigned long particle_index);

        /** returns the radius of given particle */
        double getRadius(unsigned long particle_index);

        /** returns the number of existing particles */
        unsigned long getNumberOfParticles();

        /** 
         * calculates the vector that is on the same plane as pos_vec and velocity_vector
         * that is orthogonal to pos_vec*/
        Vec3<double> calculate_ortogonal_vector_to_pos_vec(Vec3<double> pos_vec, Vec3<double> velocity_vector);

        /**
Discription:\n
Generates a new particle witch random atributes within ranges \n
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
Generates a new particle witch random atributes form range 0 to max_<atr> \n
Return value:\n
returns the index of the new Particle

*/
        unsigned long generateRandomParticle(
                double max_pos, double max_velo,
                double max_mass, double max_radius);

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
Position:\nobjekt
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

        void calculate_collision(unsigned long obj_id_1, unsigned long obj_id_2);

        void merge_objects(unsigned long obj_id_1, unsigned long obj_id_2);

        unsigned long get_vector_index(unsigned long id);

        void load_data_from_file(std::string filepath, unsigned long &number_of_previous_iterations);
        //TODO: Save funktion die die daten aus Particle in eine datei schreibt
        //
        bool check_for_collision(unsigned long id_1, unsigned long id_2);

        void detect_collision();

        void particle_bubble_sort();

        void sort_and_sweep();
        
};


#endif
