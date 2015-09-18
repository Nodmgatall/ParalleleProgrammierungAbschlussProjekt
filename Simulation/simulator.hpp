#ifndef SIMULATOR_HPP
#define SIMULATOR_HPP

#include "particle.hpp"
#include "Util/vec3.hpp"
class Simulator
{
    public:
        Simulator();
        void simulate();
        void simulate_parallel();
        void set_up_simulation();
        void get_options(int argc, char** argv);
        void setup_test(int test_id);
        std::string set_up_loading_bar();
        //void write_particle_archive_to_file();

            //option flags:
        bool m_option_load_from_file;
        bool m_output_file_entered;
        bool m_run_test;
        int m_test_id;
        //bool intersects(Vec3<double> posA, double radiusA, Vec3<double> posB, double radiusB);
        //void collide(Particle& particle);
        
    private:
        Particle m_particles;
        double m_number_of_iterations;
        unsigned long m_number_of_particles;
        unsigned long m_number_of_iterations_previous_run;
        std::string m_name_output_file;
        std::string m_name_input_file;
        std::string m_name_last_iteration_save_file;
};

#endif
