#ifndef SIMULATOR_HPP
#define SIMULATOR_HPP

#include "particle.hpp"

class Simulator
{
    public:
        Simulator();
        void simulate();
        void set_up_simulation();
        void get_options(int argc, char** argv);

        //option flags:
        bool m_option_load_from_file;
        bool m_output_file_entered;
        
    private:
        Particle m_particles;
        unsigned long m_number_of_iterations;
        unsigned long m_number_of_particles;
        double m_dt;
        std::string m_name_output_file;
        std::string m_name_input_file;
        std::string m_name_last_iteration_save_file;

};

#endif
