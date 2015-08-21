#include "simulator.hpp"
#include "physics.hpp"
#include "unistd.h"
#include "Octree/Octree.hpp"

#include <string.h>
#include <stdio.h>
#include <fstream>
Simulator::Simulator()
{
    m_option_load_from_file = false;
}
void Simulator::simulate()
{
    std::cout << "== Simulating ==" << std::endl;
    int next_bar = 1;
    std::string bars = "";

    for(int i = 0; i  < 51; i++)
    {
        bars = bars + " "; 
    }

    for(unsigned long i = 1; i < m_number_of_iterations; i++)
    {
        printf("%s(%lu / %lu)\r", bars.c_str(),i,m_number_of_iterations);
        if(i % (m_number_of_iterations / 50) == 0)
        {
            bars.at(next_bar) = '|';
            next_bar++;
        }
        fflush(stdout);

        m_particles.writePositionToFile(m_name_output_file);
        if(i == m_number_of_iterations - 1)
        {
            m_particles.writePositionToFile(m_name_last_iteration_save_file);
        }
        for (unsigned long i = 1; i < m_number_of_particles - 1; i++)
        {
            applyGravity(m_particles,i,m_dt);
            m_particles.move_Object(i,m_dt);

        }
        collide(m_particles);
    }
    std::cout << "\n== Simulation completed ==" << std::endl;
}

void Simulator::set_up_simulation()
{

    m_dt = 0.000024;
    if(m_option_load_from_file)
    {
        m_particles.load_data_from_file(m_name_input_file + ".last_iteration_save");
        std::cout << "loading of file " << m_name_input_file << " done" << std::endl;
    }
    else
    {
        m_particles.createParticle(Vec3<double>(0,0,0),
                Vec3<double>(0,0,0),
                1.98855 * pow(10, 30),
                696300000);

        m_particles.createParticle(Vec3<double>(149597870700,0,0),
                Vec3<double>(0,29780,0),
                5.972 * pow(10, 24),
                6371000.0);

        std::cout << "Generating " << m_number_of_particles << " Objects" << std::endl;

        for(unsigned long i = 0; i < m_number_of_particles - 2 ; i++)
        {
            m_particles.generateRandomParticle(149598261000 * 20, 40000,5.972 * pow(10, 24) * 6, 6371000 * 6);
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
    int c;
    bool name_output_file_set = false;
    m_name_output_file = "";
    while((c = getopt(argc, argv, "f:o:")) != -1)
    {
        switch(c)
        {
            case 'o':
                m_name_output_file = optarg;
                name_output_file_set = true;
                break;
            case 'f':
                m_name_input_file = optarg;
                m_option_load_from_file = true;
                break;

            case '?':
                if (optopt == 'c')
                    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                else if (isprint (optopt))
                {
                    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                    exit(EXIT_FAILURE);
                }
                else
                {
                    fprintf (stderr,
                            "Unknown option character `\\x%x'.\n",
                            optopt);
                    exit(EXIT_FAILURE);
                }
        }
    }
    if(argc - optind > 2)
    {
        std::cout << "some error because to many args" << std::endl;
        exit(EXIT_FAILURE);
    }

    if(argc - optind < 2)
    {
        std::cout << "some error because to less args" << std::endl;
        exit(EXIT_FAILURE);
    }

    if(!is_all_digits(argv[optind]))
    {
        std::cout << "Error: Argument for the number of particles is not all digits" << std::endl;
        exit(EXIT_FAILURE);
    }

    if(!is_all_digits(argv[optind + 1]))
    {
        std::cout << "Error: Argument for the number of iterations is not all digits" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::ifstream input_file(m_name_input_file);
    if(!(bool)input_file && m_option_load_from_file)
    {
        std::cout << "Error: File does not exists " << m_name_input_file << std::endl;
        exit(EXIT_FAILURE);
    }


    m_number_of_particles = strtoul(argv[optind], NULL, 0);
    m_number_of_iterations = strtoul(argv[optind + 1],NULL, 0);
    std::string num_objects = argv[optind];
    std::string num_iterations = argv[optind + 1];
    if(name_output_file_set)
    {
        m_name_output_file = num_objects + "-" + num_iterations + "-" + m_name_output_file + ".simdat";      
    }
    else
    {
        m_name_output_file = num_objects + "-" + num_iterations + ".simdat";       
    }
    std::ifstream output_file(m_name_output_file);
    if(output_file && !m_option_load_from_file)
    {
        std::cout << "Error: File already exists " << m_name_output_file << std::endl;
        exit(EXIT_FAILURE);
    } 
    m_name_last_iteration_save_file = m_name_output_file + ".last_iteration_save";
}


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
    std::cerr << "begin collision" << std::endl;

    unsigned long i; //iterator
    double precision = 5; // precision multiplier (must be >1)

    Octree * tree = new Octree(); //octree for organizing
    tree->setRadii(Vec3<double>(1000.0, 1000.0, 1000.0)); //TODO change this to something sensible
    std::cerr << "created Octree" << std::endl;

    // build octree
    OctreePoint * octreePoints = new OctreePoint[particle.getNumberOfParticles()];
    for (i = 0; i < particle.getNumberOfParticles(); i++) {
        octreePoints[i].setPosition(particle.getPosition(i));
        octreePoints[i].setRadius(particle.getRadius(i));
        octreePoints[i].setIndex(i);
        tree->insert(octreePoints + 1);
    }

    std::cerr << "built Octree" << std::endl;

    // determine which particles to collide with one another
    // and perform the collision
    for (i = 0; i < particle.getNumberOfParticles(); i++) {
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

        std::cerr << "lol" << std::endl;

        for (auto it = collisionPartners.begin(); it != collisionPartners.end(); ++it) {
            if (intersects(particle.getPosition(i), particle.getRadius(i), (*it)->getPosition(), (*it)->getRadius())) {
                /* do stuff */
                //std::cout << "Particle " << i << " collides with particle " << (*it)->getIndex() << std::endl;
                particle.merge_objects(i, (*it)->getIndex());
            }
        }
    }

    delete tree;
}
