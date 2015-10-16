#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include "Util/random_generator.hpp"
#include "Util/vec3.hpp"

#include <string>
#include <vector>
#include <math.h>
#include <iostream>
#include <ctime>
#include <utility> 
#include <cstdlib>
#include <map>

//#include <boost/serialization/vector.hpp>

//#include <boost/archive/binary_oarchive.hpp>
//#include <boost/archive/binary_iarchive.hpp>

//#include <boost/archive/text_oarchive.hpp>
//#include <boost/archive/text_iarchive.hpp>

// center object has to be at <0.0.0>
/**
  Data structure to store and manage particles.\n
  Paritcles are stored in a std::vector<double> and are accsessed by
  their index.
  */
class Particle
{
    private:
        std::vector<Vec3<double> > m_velocity_vectors;
        std::vector<Vec3<double> > m_positions;
        std::vector<double> m_masses;
        std::vector<double> m_radiuses;
        std::vector<unsigned long> m_ids;
        unsigned long m_number_of_particles;
        unsigned long m_max_id;
        double m_dt;
        double m_max_velo;
        unsigned long m_time_simulated;
        std::vector<Vec3<unsigned long>> m_collision_data;

    public:

        Particle();

        /**
         *  removes all data about a particle form the system, decrements m_number_of_particles
         * */
    void remove(unsigned long vector_index);

    /** returns delta time */
    double get_dt();

    /** retuns eleapsed time */
    double get_time_simulated();

    /** returns the current highest velocity */
    double get_max_velo();

    /** Vector storing the velocities ob objects */
    std::vector<Vec3<double>> get_velo_vector();

    /** Vector storing the positios ob objects */
    std::vector<Vec3<double>> get_pos_vector();

    /** Vector storing the masses ob objects */
    std::vector<double> get_mass_vector();

    /** Vector storing the radiuses of objects */
    std::vector<double> get_radius_vector();

    /** Vector storing the id's of objects */
    std::vector<unsigned long> get_id_vector();

    std::vector<Vec3<unsigned long>> get_collision_data();

    /** updates the velocity vector by overwriting it with a new one */
    void update_velo_vector(std::vector<Vec3<double>> new_velo_vector);
    /** updates the position vector by overwriting it with a new one */
    void update_pos_vector(std::vector<Vec3<double>> new_velo_vector);
    /** updates the mass vector by overwriting it with a new one */
    void update_mass_vector(std::vector<double> new_mass_vector);
    /** updates radius vector by overwriting it with a new one */
    void update_radius_vector(std::vector<double> new_radius_vector);
    /** updates the id vector by overwriting it with a new one */
    void update_id_vector(std::vector<unsigned long> new_id_vector);
    
    void update_collision_data_vector(std::vector<Vec3<unsigned long>> new_collision_data_vector);

    void set_max_velo(double new_max_velo);
    //void remove_by_id(unsigned long particle_id);

    std::pair<double,double> calculate_possible_collison_point(unsigned long index_1, unsigned long index_2);

    /** returns the distance from the center of the system */
    double get_distance_to_center(unsigned long particle_index);

    /**
     * move each object by their velocity, if the stepsize is equal to 1second 
     */
    void move_object(unsigned long particle_index);

    void move_objects(unsigned long start_idx = 1, unsigned long end_idx = 0);

    /** returns the velocity vector of given particle */
    Vec3<double> get_velocity_vector(unsigned long particle_index);

    /** adds a acceleration vector to the velocity vectof of specified object*/
    void add_acceleration_vector(unsigned long particle_index, Vec3<double> accelerationVector);

    /** returns the positon of given particle */
    Vec3<double> getPosition(unsigned long particle_index);

    /** returns a pointer to a copied m_positions Vector for saving */ 
    Vec3<Vec3<double> > savePositionData();

    void write_to_file(std::string filename, unsigned long iteration_number, std::ios_base::openmode mode) ;
    /** returns the mass of given particle */
    double get_mass(unsigned long particle_index);

    /** returns the radius of given particle */
    double get_radius(unsigned long particle_index);

    /** returns the number of existing particles */
    unsigned long get_number_of_particles();

    /** 
     * calculates the vector that is on the same plane as pos_vec and velocity_vector
     * that is orthogonal to pos_vec
     */
    Vec3<double> calculate_ortogonal_vector_to_pos_vec(Vec3<double> pos_vec, Vec3<double> velocity_vector);

    /**
     * Discription:\n
     * Generates a new particle witch random atributes within ranges \n
     * Return value:\n
     * returns the index of the new Particle
     */

    unsigned long generate_random_object(
            std::pair<double,double> range_position,
            std::pair<double,double> range_velocity,
            std::pair<double,double> range_mass,
            std::pair<double,double> range_radius);
    /**	
     * Discription:\n
     * Generates a new particle witch random atributes form range 0 to max_<atr> \n
     * Return value:\n
     * returns the index of the new Particle
     */

    unsigned long generate_random_object(
            double max_pos, double max_velo,
            double max_mass, double max_radius);

    /**
     * * Generates a new object with specified values\n
     * Return value:\n
     * returns the index of the new Particle
     */

    unsigned long create_object(
            Vec3<double> positon,
            Vec3<double> velocity_vector,
            double mass,
            double radius);

    /** prints all data about given particle to the console 
     *        Example output:\n
     * ParicleID: 0\n
     * Distance from center: 58984654.079628\n
     * Position:\nobjekt
     * X: 2656945.474053\n
     * Y: 46291973.436072\n
     * Z: 36457965.563581\n
     * Velocity: 1.805316\n
     * X: 0.730751\n
     * Y: 1.311961\n
     * Z: 1.001961\n
     * Mass: 2255.501134\n
     * Radius 429.489498\n
     */
    void print_object(unsigned long particle_index);

    /** prints all particles to the console */
    void print_all_objects();


    /**
     *  Iterates over all objects and calls applyGravity() (from physics.hpp) for each
     * */
    void apply_gravity_loop(unsigned long start_idx = 1, unsigned long end_idx = 0); 

    /** */
    void calculate_collision(unsigned long obj_id_1, unsigned long obj_id_2);

    /**
     * merges 2 kolliding objects and replaces the one that is nearer to the center of
     * the world with a new one, deletes the second one, effectively shrinking the number of objects by one.
     * A Exeption is if a objects collides with the sun or the main center of gravity
     * (always pos 0), here only the object is deletet and no new object is generated.
     * 
     * */
    void merge_objects(unsigned long obj_id_1, unsigned long obj_id_2);

    /** loads data from earlier run */
    void load_data_from_file(std::string filepath, unsigned long &number_of_previous_iterations);

    /** returns true if distance between two objects is smaller than the sum of their radiuses */
    bool check_for_collision(unsigned long id_1, unsigned long id_2, double time_of_closest_approach);

    /** iterates over all particles and checks for collisions */
    void detect_collision(unsigned long index_1 = 1, unsigned long index_2 = 0);

    /** Sorts particle data for distance to sun*/
    void particle_bubble_sort(unsigned long start_idx = 0, unsigned long end_idx = 0);

    /** checks if distances from sun are greater than 2 x max_velo 
     * retuns true if distance is greater than 2x max velo
     * */
#ifdef PARALLEL_BUILD
    void broadcast_object_data(unsigned long size);
#endif
    bool limit(unsigned long index_1, unsigned long index_2);
};


#endif
