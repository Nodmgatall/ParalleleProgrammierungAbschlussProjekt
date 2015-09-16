#include "simulator.hpp"
#include "physics.hpp"
#include "unistd.h"
#include "Octree/Octree.hpp"

#include "Util/macros.hpp"

#include <string.h>
#include <stdio.h>
#include <fstream>

const double AU = 149597870700.0;  
Simulator::Simulator()
{
    m_option_load_from_file = false;
    m_number_of_iterations_previous_run = 0;
}

std::string set_up_loading_bar()
{
    std::string bars = "";
    for(int i = 0; i  < 51; i++)
    {
        bars = bars + " "; 
    }

    return bars;
}

/**
 * Starts the simulation
 * */
void Simulator::simulate()
{
    unsigned long current_iteration = 0;
    std::cout << "== Simulating ==" << std::endl;
 

        
    // Simulation loop
    while(m_particles.get_time_simulated() < m_number_of_iterations)
    {
        //std::cout << "im here" << std::endl;
        //Print loading bar
        printf("(%lu / %lu) %lu \r",(unsigned long)m_particles.get_time_simulated(), (unsigned long)m_number_of_iterations, current_iteration);
       

        fflush(stdout);
        
        //Actual simulation
        
        m_particles.apply_gravity(1);
        m_particles.move_objects(1);
        m_particles.sort_and_sweep();
        
        //Wrtite simulation data to file and in last iteration save last iteration
        if(current_iteration == m_number_of_iterations - 1)
        {
            m_particles.write_to_file(m_name_last_iteration_save_file, current_iteration + m_number_of_iterations_previous_run, std::ofstream::trunc | std::ofstream::binary);
        }
        //write_particle_archive_to_file();    

        m_particles.write_to_file(m_name_output_file, current_iteration + m_number_of_iterations_previous_run, std::ofstream::app | std::ofstream::binary);
        current_iteration++;
    }
    std::cout << "\n== Simulation completed ==" << std::endl;
    }

/**
 * Generates random objects or loads data.
 *
 * */
void Simulator::set_up_simulation()
{
    if(m_option_load_from_file)
    {
        std::cout << "loading of file " << m_name_input_file << " done" << std::endl;
        m_particles.load_data_from_file(m_name_input_file, m_number_of_iterations_previous_run);
        m_number_of_iterations_previous_run++;
    }
    else if(m_run_test)
    {
        setup_test(m_test_id);
        std::cout << "test setup done" << std::endl;
    }

    else
    {
        //SUN
        m_particles.createParticle(Vec3<double>(0,0,0),
                Vec3<double>(0,0,0),
                1.98855 * pow(10, 30),
                696300000);
        //EARTH
        m_particles.createParticle(Vec3<double>(AU,0,0),
                Vec3<double>(0,29780,0),
                5.972 * pow(10, 24),
                6371000.0);

        //Generation of random objects
        std::cout << "Generating " << m_number_of_particles << " Objects" << std::endl;

        for(unsigned long i = 0; i < m_number_of_particles - 2 ; i++)
        {
            m_particles.generateRandomParticle(AU , 20000,5.972 * pow(10, 24) * 6, 6371000 * 15);
        }
        std::cout << "Generating done" << std::endl;
    }

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
    if (argc > 4)
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
        }
        input_file.close();
        m_number_of_iterations = strtod(argv[3],NULL);
    }
    else if(strcmp(argv[1], "test") == 0)
    {
        m_run_test = true;
        m_option_load_from_file = false;

        if(!is_all_digits(argv[2]))
        {
            std::cout << "ERROR: argument 2 ["<< argv[1] <<"] contains non digit parts" << std::endl;
            exit(EXIT_FAILURE);
        } 
        std::string test_id = argv[2];
        m_test_id = atoi(argv[2]);
        m_name_output_file = "test_run" + test_id;
        std::cout << m_name_output_file << std::endl;
    }
    else
    {
        m_option_load_from_file = false;
        if(!is_all_digits(argv[1]))
        {
            std::cout << "ERROR: argument 2 ["<< argv[1] <<"] contains non digit parts" << std::endl;
            exit(EXIT_FAILURE);
        }
        m_number_of_iterations = atof(argv[2]);
        std::cout << m_number_of_iterations << std::endl;
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
// THIS IS SP... , i mean, DEAD CODE!
/*
bool Simulator::intersects(Vec3<double> posA, double radiusA, Vec3<double> posB, double radiusB)
{
    // declare a bunch of variables to de-clutter the actual equation
    double AxMin = posA.getX() - radiusA;
    double AxMax = posA.getX() + radiusA;
    0double AyMin = posA.getY() - radiusA;
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
set_up_simulation
*/

//marke;

void Simulator::setup_test(int test_id)
{
    switch(test_id)
    {
        m_number_of_iterations = 60* 60 *24 * 100;
        m_number_of_particles = 3;
        case 1:
            m_particles.createParticle(Vec3<double>(0,0,0),
                    Vec3<double>(0,0,0),
                    0,
                    0);

            //EARTH
            m_particles.createParticle(Vec3<double>(AU,0,0),
                    Vec3<double>(10000,0,0),
                    5.972 * pow(10, 24),
                    6371000.0);
            //EARTH
            m_particles.createParticle(Vec3<double>(-AU,0,0),
                    Vec3<double>(-10000,0,0),
                    5.972 * pow(10, 24),
                    6371000.0);

            break;
    }
}

//void Simulator::write_particle_archive_to_file()
//{
//    std::ofstream ofs("archive.test", std::ios::app | std::ios::binary);
//    boost::archive::binary_oarchive oa(ofs,boost::archive::no_header);
//    double test = m_particles.get_time_simulated();
//    oa << test;
//    oa << m_number_of_iterations; 
//    m_particles.write_to_archive(&oa);
//}
