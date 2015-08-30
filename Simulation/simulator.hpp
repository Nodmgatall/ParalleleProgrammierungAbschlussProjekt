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
        template<class Archive>
        void serialize(Archive & archive);
        void archive_particles();

            //option flags:
        bool m_option_load_from_file;
        bool m_output_file_entered;
        bool intersects(Vec3<double> posA, double radiusA, Vec3<double> posB, double radiusB);
        void collide(Particle& particle);
        
    private:
        Particle m_particles;
        unsigned long m_number_of_iterations;
        unsigned long m_number_of_particles;
        unsigned long m_number_of_iterations_previous_run;
        double m_dt;
        std::string m_name_output_file;
        std::string m_name_input_file;
        std::string m_name_last_iteration_save_file;
};

#endif
