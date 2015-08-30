#include "simulator.hpp"
#include "physics.hpp"
#include "unistd.h"
#include <string.h>
#include <stdio.h>
#include <fstream>
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
    std::cout << "== Simulating ==" << std::endl;
 
    // Setup for progress bar
    int next_bar = 1;
    std::string bars = "";

    for(int i = 0; i  < 51; i++)
    {
        bars = bars + " "; 
    }
    
    // Simulation loop
    for(unsigned long current_iteration = 1; current_iteration < m_number_of_iterations; current_iteration++)
    {
        //Print loading bar
        printf("%s(%lu / %lu)\r", bars.c_str(),current_iteration, m_number_of_iterations);
        
        //Change loading bar to represent simulation progress
        if(m_number_of_iterations >= 50 && current_iteration % (m_number_of_iterations / 50) == 0) 
        {
            bars.at(next_bar) = '|';
            next_bar++;
        }
        fflush(stdout);
        
        //Actual simulation
        for (unsigned long particle_index = 1; particle_index < m_particles.getNumberOfParticles() - 1; particle_index++)
        {
            applyGravity(m_particles,particle_index,m_dt);
            m_particles.move_Object(particle_index,m_dt);
            m_particles.detect_collision();
        }
        
        //Wrtite simulation data to file and in last iteration save last iteration
        if(current_iteration == m_number_of_iterations - 1)
        {
            m_particles.write_to_file(m_name_last_iteration_save_file, current_iteration + m_number_of_iterations_previous_run, std::ofstream::trunc | std::ofstream::binary);
        }

        m_particles.write_to_file(m_name_output_file, current_iteration + m_number_of_iterations_previous_run, std::ofstream::app | std::ofstream::binary);
    }
    std::cout << "\n== Simulation completed ==" << std::endl;
}
/**
 * Generates random objects or loads data.
 *
 * */
void Simulator::set_up_simulation()
{
    m_dt = 0.000024;
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
