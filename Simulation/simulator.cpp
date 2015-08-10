#include "simulator.hpp"
#include "physics.hpp"
#include "unistd.h"

#include <string.h>
#include <fstream>
Simulator::Simulator()
{
    m_option_load_from_file = false;
}
void Simulator::simulate()
{
    for(unsigned long i = 0; i < m_number_of_iterations; i++)
    {
        m_particles.writePositionToFile(m_name_output_file);
        if(i == m_number_of_iterations - 1)
        {
            m_particles.writePositionToFile(m_name_last_iteration_save_file);
        }
        std::cout << "Iteration " << i << " of" << m_number_of_iterations << std::endl;
        for (unsigned long i = 1; i < m_number_of_particles - 1; i++)
        {
            applyGravity(m_particles,i,m_dt);
            m_particles.move_Object(i,m_dt);

        }
    }
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
                10.0);
        m_particles.createParticle(Vec3<double>(149598261000,0,0),
                Vec3<double>(0,29780,0),
                5.972 * pow(10, 24),
                10.0);

        std::cout << "Generating " << m_number_of_particles << " Objects" << std::endl;

        for(unsigned long i = 0; i < m_number_of_particles - 2 ; i++)
        {
            m_particles.generateRandomParticle(149598261000 * 20, 40000,5.972 * pow(10, 24) * 6, 100000000000000000);
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
    while((c = getopt(argc, argv, "f:")) != -1)
    {
        switch(c)
        {
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
    if(argc - optind > 3)
    {
        std::cout << "some error because to many args" << std::endl;
        exit(EXIT_FAILURE);
    }

    if(argc - optind < 3)
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
    std::ifstream output_file(argv[optind + 2]);
    if(output_file && !m_option_load_from_file)
    {
        std::cout << "Error: File already exists " << argv[optind + 2] << std::endl;
        exit(EXIT_FAILURE);
    }
    m_name_output_file = argv[optind + 2];
    m_name_last_iteration_save_file = m_name_output_file + ".last_iteration_save";
    std::cout << argv[optind] << " " << argv[optind + 1] << " " << argv[ optind + 2] << " " << m_name_output_file << " " << m_name_last_iteration_save_file << " " << m_name_input_file <<  std::endl;
    
}
