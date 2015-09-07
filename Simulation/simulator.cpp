#include "simulator.hpp"
#include "physics.hpp"
#include "unistd.h"
#include "Octree/Octree.hpp"

#include "Util/macros.hpp"

#include "globals.h"

#include <string.h>
#include <stdio.h>
#include <fstream>

#include <boost/mpi.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>

/*
 * Definitions for MPI sending
 * These are pretty much arbitrary and not entirely necessary,
 * but they make the code more readable
 */
#define VELOCITY_TAG 0
#define POSITION_TAG 1
#define MASS_TAG 2
#define RADIUS_TAG 3
#define ID_TAG 4
#define ID_DEL_TAG 5
#define INDEX_TAG 6

Simulator::Simulator()
{
    m_option_load_from_file = false;
    m_number_of_iterations_previous_run = 0;
}

/**
 * Starts the simulation
 * */
void Simulator::simulate()
{
    DEBUG("Begin simulation!");

    /*
     * Utility variables
     * (iterators and whatnot)
     */
    int i;

    /*
     * Sending and receiving variables
     */
    unsigned long receive_particle_index;
    Vec3<double> receive_velocity;
    Vec3<double> receive_position;
    double receive_mass;
    double receive_radius;
    //unsigned long receive_id;
    //unsigned long receive_id_deleted;
    //std::vector<Vec3<double>> receive_velocities;
    //std::vector<Vec3<double>> receive_positions;
    //std::vector<double> receive_masses;
    //std::vector<double> receive_radiuses;
    //std::vector<unsigned long> receive_ids;
    //std::vector<unsigned long> receive_ids_deleted;

    /*
     * Aggregation variables
     * FYI: to append vector b to vector a:
     *      a.insert(a.end(), b.begin(), b.end());
     */
    std::vector<Vec3<double> > aggregate_velocities;
    std::vector<Vec3<double> > aggregate_positions;
    std::vector<double> aggregate_masses;
    std::vector<double> aggregate_radiuses;
    std::vector<unsigned long> aggregate_ids;
    std::vector<unsigned long> aggregate_ids_deleted;
 
    /*
     * MPI communicator
     * default constructor creates a communicator that contains all processes
     */
    boost::mpi::communicator world; // basically equivalent to MPI_COMM_WORLD

    /* 
     * calculate number of particles simulated by each rank 
     */
    unsigned long particles_per_rank = m_particles.getNumberOfParticles() / worldsize; //integer division!
    unsigned long rest = m_particles.getNumberOfParticles() % worldsize; // remaining particles
    unsigned long start_particle; // particle that this rank start simulating at
    unsigned long end_particle; // particle that this rank stops simulating at

    /* example calculation (in sort-of pseudo-code):
     * 100 particles
     * 10 processes
     * start_particle[rank0] = 0 * 10 + 1 = 1; checks out, because we ignore the sun at particles[0]
     * end_particle[rank0] = (0 + 1) * 10 = 10; checks out as well
     *
     * start_particle[rank1] = 1 * 10 + 1 = 11; picks up where last rank left off, perfect.
     * end_particles[rank1] = (1 + 1) * 10 = 20; simulates same number of particles as rank 0
     *
     * This can be continued for all ranks, with the result that all particles will be simulated.
     * The formula checks out. See below for solution for particles % processes != 0
     */
    start_particle = rank * particles_per_rank + 1;
    end_particle = (rank + 1) * particles_per_rank;

    // primitively add remaining particles to be simulated by last rank
    // TODO: do this less primitively! Although dividing these evenly across ranks
    // is a bit of a pain, it can probably increase performance by quite a bit,
    // given an uncompatible particle/process ratio
    if (rank == worldsize - 1)
    {
        end_particle += rest;
    }
    
    // Simulation loop
    // TODO: how does every process know when to end?
    for(unsigned long current_iteration = 1; current_iteration < m_number_of_iterations; current_iteration++)
    {
        DEBUG("Simulating iteration " << current_iteration);
        
        // Simulation
        for (unsigned long particle_index = start_particle; particle_index <= end_particle; particle_index++)
        {
            // TODO: parallelize
            // DONE: the function itself does not need to be parallelized
            //       this loop is divided evenly across processes
            applyGravity(m_particles,particle_index,m_dt);

            // TODO: parallelize
            // DONE: same as applyGravity()
            m_particles.move_Object(particle_index,m_dt);

            // if we're any rank but 0, send our computed information
            if (rank != 0)
            {
                world.send(0, INDEX_TAG, particle_index);
                world.send(0, VELOCITY_TAG, m_particles.getVelocityVector(particle_index));
                world.send(0, POSITION_TAG, m_particles.getPosition(particle_index));
                world.send(0, MASS_TAG, m_particles.getMass(particle_index));
                world.send(0, RADIUS_TAG, m_particles.getRadius(particle_index));
                //world.send(0, ID_TAG, m_particles.getID(particle_index));
                //world.send(0, ID_DEL_TAG, m_particles.getDeletedIDs());
            }
            // if we're rank 0, receive it all, obviously
            else if (rank == 0 && worldsize > 1)
            {
                for (i = 1; i < worldsize; ++i)
                {
                    // receive index to alter
                    world.recv(i, INDEX_TAG, receive_particle_index);

                    // receive and alter velocity
                    world.recv(i, VELOCITY_TAG, receive_velocity);
                    m_particles.setVelocityVector(receive_particle_index, receive_velocity);

                    // receive and alter position
                    world.recv(i, POSITION_TAG, receive_position);
                    m_particles.setPosition(receive_particle_index, receive_position);
                    
                    // receive and alter mass
                    world.recv(i, MASS_TAG, receive_mass);
                    m_particles.setMass(receive_particle_index, receive_mass);

                    // receive and alter radius
                    world.recv(i, RADIUS_TAG, receive_radius);
                    m_particles.setRadius(receive_particle_index, receive_radius);

                    // receive and alter id
                    // world.recv(i, ID_TAG, receive_id);
                    // m_particles.setID(particle_index, receive_id);
                }
                m_particles.sort_and_sweep();
                broadcast(world, m_particles, 0);
            }
            else // we're not actually doing anything in parallel here, since we seem to only have 1 process...
            { /* do nothing */ }

            // TODO
        }
        
        //i Write simulation data to file and in last iteration save last iteration
        // TODO: only do this as #0
        if (rank == 0)
        {
            if(current_iteration == m_number_of_iterations - 1)
            {
                m_particles.write_to_file(m_name_last_iteration_save_file, current_iteration + m_number_of_iterations_previous_run, std::ofstream::trunc | std::ofstream::binary);
            }

            m_particles.write_to_file(m_name_output_file, current_iteration + m_number_of_iterations_previous_run, std::ofstream::app | std::ofstream::binary);
        }

        world.barrier();
    }
    DEBUG("Simulation complete!");
}

/**
 * Generates random objects or loads data.
 *
 * */
void Simulator::set_up_simulation()
{
    // only rank 0 sets up the simulation
    // if another rank has gotten this far, something went wrong
    if (rank != 0)
    {
        std::cerr << "set_up_simulation() was called by unauthorized process with rank "
            << rank << std::endl;
        boost::mpi::environment::abort(EXIT_FAILURE);
    }

    m_dt = 0.00024;

    // TODO: this might go wrong on the cluster
    if(m_option_load_from_file)
    {
        std::cout << "loading of file " << m_name_input_file << " done" << std::endl;
        m_particles.load_data_from_file(m_name_input_file, m_number_of_iterations_previous_run);
        m_number_of_iterations_previous_run++;
    }
    else
    {
        //SUN
        m_particles.createParticle(Vec3<double>(0,0,0),
                Vec3<double>(0,0,0),
                1.98855 * pow(10, 30),
                696300000);
        //EARTH
        m_particles.createParticle(Vec3<double>(149597870700,0,0),
                Vec3<double>(0,29780,0),
                5.972 * pow(10, 24),
                6371000.0);

        //Generation of random objects
        std::cout << "Generating " << m_number_of_particles << " Objects" << std::endl;

        for(unsigned long i = 0; i < m_number_of_particles - 2 ; i++)
        {
            m_particles.generateRandomParticle(149598261000, 40000,5.972 * pow(10, 24) * 6, 6371000 * 6);
        }
        DEBUG("Simulation set-up: particle generation done!");
    }

    // calculate size of m_particles for debugging purposes
#ifdef DEBUG_BUILD
    unsigned long total_size = 0;
    std::vector<Vec3<double> > vel_vecs = m_particles.getVelocities();
    for (std::vector<Vec3<double> >::iterator it = vel_vecs.begin(); it != vel_vecs.end(); ++it)
    {
        total_size += sizeof(Vec3<double>);
    }
    total_size *= 2; // approximate addition of positions
    total_size += m_particles.getNumberOfParticles() * sizeof(double) * 2;
    total_size += m_particles.getNumberOfParticles() * sizeof(unsigned long) * 2;
    total_size += sizeof(unsigned long) * 2;

    std::cerr << "Total size of Particle object: " << total_size << std::endl;
#endif
    // send particles to other processes
    // TODO: works fine for <56 particles
    //       seems to produce a deadlock for >=56 particles
    //       ?????????????
    boost::mpi::communicator world;
    broadcast(world, m_particles, 0);
    DEBUG("Successfully broadcast particles to other processes!");
    DEBUG("Setup complete!");
}

bool is_all_digits(char *text)
{

    int string_length = strlen(text); 

    for(int i = 0; i < string_length - 2; i++)
    {
        if(!isdigit(text[i]))
        {
            return false;
        }
    }
    return true;
}


void Simulator::get_options(int argc, char** argv)
{
    if (argc != 4)
    {
        std::cerr << "Error: invalid number of arguments!" << std::endl;
        exit(EXIT_FAILURE);
    }

    if(strcmp(argv[1], "load") == 0)
    {       
        m_option_load_from_file = true;
        m_name_input_file = argv[2];
        m_name_output_file = m_name_input_file;
        m_name_input_file += ".lis";
        std::ifstream input_file(m_name_input_file);
        if(!(bool)input_file)
        {
            std::cout << "Error: File does not exists " << m_name_input_file << std::endl;
            exit(EXIT_FAILURE);
        }input_file.close();
        m_number_of_iterations = strtoul(argv[3],NULL,0);
    }
    else
    {
        m_option_load_from_file = false;
        if(!is_all_digits(argv[1]))
        {
            std::cout << "ERROR: argument 2 ["<< argv[1] <<"] contains non digit parts" << std::endl;
            exit(EXIT_FAILURE);
        }
        m_number_of_iterations = strtoul(argv[2],NULL,0);
        m_number_of_particles = strtoul(argv[1],NULL,0);
        m_name_output_file = argv[3];
        std::ifstream out_file(m_name_output_file);
        if((bool)out_file)
        {
            std::cout << "Error: File already exists " << m_name_input_file << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    m_name_last_iteration_save_file = m_name_output_file + ".lis";

}

/*
bool Simulator::intersects(Vec3<double> posA, double radiusA, Vec3<double> posB, double radiusB)
{
    // declare a bunch of variables to de-clutter the actual equation
    double AxMin = posA.getX() - radiusA;
    double AxMax = posA.getX() + radiusA;
    double AyMin = posA.getY() - radiusA;
    double AyMax = posA.getY() + radiusA;
    double AzMin = posA.getZ() - radiusA;
    double AzMax = posA.getZ() + radiusA;

    double BxMin = posB.getX() - radiusB;
    double BxMax = posB.getX() + radiusB;
    double ByMin = posB.getY() - radiusB;
    double ByMax = posB.getY() + radiusB;
    double BzMin = posB.getZ() - radiusB;
    double BzMax = posB.getZ() + radiusB;

    // calculate result
    return (AxMin < BxMax && AxMax > BxMin) &&
           (AyMin < ByMax && AyMax > ByMin) &&
           (AzMin < BzMax && AzMax > BzMin);
}


void Simulator::collide(Particle& particle)
{
    DEBUG("Begin collision...");

    unsigned long i; //iterator
    double precision = 5; // precision multiplier (must be >1)

    Octree * tree = new Octree(); //octree for organizing
    double lim = 1.0 * 1000.0 * 1000.0 * 1000.0 * 1000.0;
    tree->setRadii(Vec3<double>(lim, lim, lim)); //TODO change this to something sensible
    DEBUG("Created octree...");

    // build octree
    OctreePoint * octreePoints = new OctreePoint[particle.getNumberOfParticles()];
    for (i = 0; i < particle.getNumberOfParticles(); i++)
    {
        octreePoints[i].setPosition(particle.getPosition(i));
        octreePoints[i].setRadius(particle.getRadius(i));
        octreePoints[i].setIndex(i);
        tree->insert(octreePoints + i);
    }

    DEBUG("Assembled octree...");

    // determine which particles to collide with one another
    // and perform the collision
    for (i = 0; i < particle.getNumberOfParticles(); i++)
    {
        // variable to save all points to collide with
        std::vector<OctreePoint*> collisionPartners;

        // help variable
        double radius = particle.getRadius(i);
        
        // vectors for calculating box in which to consider particles
        Vec3<double> radius_vec = Vec3<double>(radius, radius, radius);
        Vec3<double> bmin = Vec3<double>() + particle.getPosition(i) - radius_vec * precision;
        Vec3<double> bmax = Vec3<double>() + particle.getPosition(i) + radius_vec * precision;
        
        // push all points within i's vicinity into this vector
        tree->getPointsInBox(bmin, bmax, collisionPartners);

        for (auto it = collisionPartners.begin(); it != collisionPartners.end(); ++it)
        {
            if (i == (*it)->getIndex()) continue; // things can't collide with themselves

            if (intersects(particle.getPosition(i), particle.getRadius(i), (*it)->getPosition(), (*it)->getRadius()))
            {
                DEBUG("Particle " << i << " collides with particle " << (*it)->getIndex() << "...");
                DEBUG("Particle " << i << "'s position: " << particle.getPosition(i).toString());
                DEBUG("Particle " << (*it)->getIndex() << "'s position: " << (*it)->getPosition().toString());

                // When two particles collide, they merge. This is somewhat primitive but also somewhat realistic.
                particle.merge_objects(i, (*it)->getIndex());
            }
        }
        DEBUG("Collision done for particle " << i << "!");
    }

    DEBUG("Collision done!");

    delete tree;
}

*/
