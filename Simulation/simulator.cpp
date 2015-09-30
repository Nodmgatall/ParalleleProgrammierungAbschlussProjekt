#include "simulator.hpp"
#include "physics.hpp"
#include "unistd.h"
#include "Octree/Octree.hpp"
#include "Util/vec3.hpp"

#ifdef PARALLEL_BUILD
#include "globals.h"
#include <mpi.h>
#endif

#include "Util/macros.hpp"

#include <string.h>
#include <stdio.h>
#include <fstream>

const double AU = 149597870700.0;  
Simulator::Simulator()
{
    m_option_load_from_file = false;
    m_number_of_iterations_previous_run = 0;
    m_run_test = false;
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
std::vector<int> calculate_chunk_size(int number_of_procs, int buffer_size)
{
    int chunk_size= buffer_size / number_of_procs;
    int rest = buffer_size - chunk_size * number_of_procs;
    std::vector<int> chunks(number_of_procs - 1);
    for(int i = 0; i < number_of_procs; i++)
    {
        if(rest > 0)
        {
            chunks[i] = chunk_size + 1;
            rest--;
        }
        else
        {
           chunks[i] = chunk_size;
        }
#ifdef OUTPUT_DEBUG
        std::cout << "Chunk for pro : "<< i << "  "<< chunks[i] <<  std::endl;
#endif
    }
    return chunks;
}


/*
 *
 * Funktions weise (grob):
 *
 *  ALLE:
 *      - variablen init am anfang
 *      - erhoen des privaten counters am ende
 *  PRO 0:
 *      - pro 0 errechnet die anteile an den daten vektoren von m_particles
 *      - sortiert die vektoren nach der entfernung zum mittelpunkt 
 *              (TODO: diesen schritt komplett abkoppeln und dann parallelisieren mit mergesort als letzen schritt und pro 1 - n mit bubble_sort)
 *      - broadcastet particle anzahl durch das senden der size vom pos vektor
 *      - broadcastet die kompletten pos, velo, mass, radius vektoren
 *      - erhaelt dann in richtiger reinfolge die neu berechneten daten und fuegt sie wieder zusammen
 *      - ueberscheibt seine alten daten mit den neuen
 *      - gibt sie aus
 *  PRO 1 - n
 *      - empfaengt die broadcasts von 0
 *      - erechnet seine start und end positionen in den vektoren
 *      - berechnet collisionen 
 *      - berechnet die neunen geschwindigkeiten
 *      - berechnet die neuen positionen
 *      - sendet neue positionen
 *      - sendet neue geschwindigkeiten
 * */
#ifdef PARALLEL_BUILD 
void Simulator::simulate_parallel()
{
    int tag, source, destination;
    unsigned long object_count;
    int number_of_procs;
    int pro_id;
    MPI_Status status;
    Vec3<double>::create_mpi_type();
    std::vector<int> chunk_sizes;
    unsigned long size;

    unsigned long current_iteration = 0;
    MPI_Comm_size(MPI_COMM_WORLD, &number_of_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &pro_id);
    tag = 1;

    if(pro_id == 0)
    {
        std::cout << "== Simulating ==" << std::endl;
    }


    // Simulation loop
    while(current_iteration < m_number_of_iterations)
    {
        
        if(pro_id == 0)
        {
            printf("(%lu / %lu) %lu \r",
                    (unsigned long)m_particles.get_time_simulated(),
                    (unsigned long)m_number_of_iterations, current_iteration);


            fflush(stdout);


            //Size equals number of particles
            size = m_particles.get_velo_vector().size();

            chunk_sizes = calculate_chunk_size(number_of_procs - 1, size);
            
            m_particles.particle_bubble_sort();

          
            MPI::COMM_WORLD.Bcast(&size,1,MPI_UNSIGNED_LONG,0);
            MPI::COMM_WORLD.Bcast(&m_number_of_iterations,1,MPI_UNSIGNED_LONG,0);

#ifdef OUTPUT_DEBUG
            std::cout << pro_id << ": chunsize/sort/set_size done" << std::endl;
            std::cout << pro_id << ": starting sending object data(chuck size)" << std::endl;
#endif          
            //Sending chunksizes
            MPI_Bcast(&chunk_sizes[0],number_of_procs - 1,
                    MPI_INT,
                    0,
                    MPI_COMM_WORLD);
            //Sending velocities vector
#ifdef OUTPUT_DEBUG
            std::cout << pro_id << ": starting sending object data(velo)" << std::endl;
#endif
            MPI::COMM_WORLD.Bcast(&m_particles.get_velo_vector()[0],
                    size,
                    MPI_Vec3,
                    0);

            //Sending positions vector
#ifdef OUTPUT_DEBUG
            std::cout << pro_id << ": starting sending object data(pos)" << std::endl;
#endif
            MPI::COMM_WORLD.Bcast(&m_particles.get_pos_vector()[0],
                    size,
                    MPI_Vec3,
                    0);

            // Sending mass vector
#ifdef OUTPUT_DEBUG
            std::cout << pro_id << ": starting sending object data(vector)" << std::endl;
#endif
            MPI::COMM_WORLD.Bcast(&m_particles.get_mass_vector()[0],
                    size,
                    MPI_DOUBLE,
                    0);
            //Sending radius vector
#ifdef OUTPUT_DEBUG
            std::cout << pro_id << ": starting sending object data(radius)" << std::endl;
#endif
            MPI::COMM_WORLD.Bcast(&m_particles.get_radius_vector()[0],
                    size,
                    MPI_DOUBLE,
                    0);
#ifdef OUTPUT_DEBUG
            std::cout << pro_id << ": starting sending object data(id)" << std::endl;
#endif

            MPI::COMM_WORLD.Bcast(&m_particles.get_id_vector()[0],
                    size,
                    MPI_DOUBLE,
                    0);
#ifdef OUTPUT_DEBUG
            std::cout << pro_id << ": sending object data done\n " << std::endl;
#endif
    
            
            //Receive velo vector
            std::vector<Vec3<double> > new_velocity_vector;
            std::vector<Vec3<double> > new_pos_vector;
            std::vector<double> new_mass_vector;
            std::vector<double> new_radius_vector;
            std::vector<unsigned long> new_id_vector;

#ifdef OUTPUT_DEBUG        
            std::cout << pro_id << ": " << std::endl;
            std::cout << pro_id << ": starting receiveing" << std::endl;
#endif           
            
            for(int i = 1; i < number_of_procs; i++)
            {
                unsigned long new_size;
                MPI::COMM_WORLD.Recv(&new_size,1,MPI_UNSIGNED_LONG, i, tag);
                std::vector<Vec3<double>> v3_buffer(new_size);
                std::vector<double> d_buffer(new_size);
                std::vector<double> ul_buffer(new_size);
        
                MPI::COMM_WORLD.Recv(&v3_buffer[0], new_size,MPI_Vec3,i,tag);
                new_velocity_vector.insert(new_velocity_vector.end(),v3_buffer.begin(),v3_buffer.end());
                
                MPI::COMM_WORLD.Recv(&v3_buffer[0], new_size,MPI_Vec3,i,tag);
                new_pos_vector.insert(new_pos_vector.end(),v3_buffer.begin(),v3_buffer.end());
                
                MPI::COMM_WORLD.Recv(&d_buffer[0], new_size,MPI_Vec3,i,tag);
                new_radius_vector.insert(new_radius_vector.end(),d_buffer.begin(),d_buffer.end());
                
                MPI::COMM_WORLD.Recv(&d_buffer[0], new_size,MPI_Vec3,i,tag);
                new_mass_vector.insert(new_mass_vector.end(),d_buffer.begin(),d_buffer.end());
        
                MPI::COMM_WORLD.Recv(&ul_buffer[0], new_size,MPI_Vec3,i,tag);
                new_id_vector.insert(new_id_vector.end(),ul_buffer.begin(),ul_buffer.end());
            }
#ifdef OUTPUT_DEBUG
            std::cout << pro_id << ": done receiving" << std::endl;
            std::cout << pro_id << ": starting update\n" << std::endl;
    
#endif

            m_particles.update_velo_vector(new_velocity_vector);
            m_particles.update_pos_vector(new_pos_vector);    

#ifdef OUTPUT_DEBUG
            std::cout << pro_id << ": done receiving" << std::endl;
            std::cout << pro_id << ": starting update" << std::endl;    
#endif

            m_particles.update_velo_vector(new_velocity_vector);
            m_particles.update_pos_vector(new_pos_vector);     

#ifdef OUTPUT_DEBUG
            std::cout << pro_id << ": update done\n" << std::endl;
            std::cout << pro_id << ": starting file write" << std::endl;
#endif
            if(current_iteration == m_number_of_iterations - 1)
            {
                m_particles.write_to_file(m_name_last_iteration_save_file, current_iteration + m_number_of_iterations_previous_run, std::ofstream::trunc | std::ofstream::binary);
            }
            //write_particle_archive_to_file();    

            m_particles.write_to_file(m_name_output_file, current_iteration + m_number_of_iterations_previous_run, std::ofstream::app | std::ofstream::binary);
#ifdef OUTPUT_DEBUG
            std::cout << pro_id << ": file write done\n\n" << std::endl; 
#endif
        }
        else
        {
            // std::cout << pro_id << " starting new iteration: " << current_iteration << std::endl;
            MPI::COMM_WORLD.Bcast(&size,1,MPI_UNSIGNED_LONG,0);
            MPI::COMM_WORLD.Bcast(&m_number_of_iterations,1,MPI_UNSIGNED_LONG,0);

            //Receive chunksize vector
            chunk_sizes = std::vector<int>(number_of_procs - 1);
            MPI::COMM_WORLD.Bcast(&chunk_sizes[0],number_of_procs - 1,
                    MPI_INT,
                    0);

            //Receive velo vector
            std::vector<Vec3<double>> velo_buffer(size);
            MPI::COMM_WORLD.Bcast(&velo_buffer[0],
                    size, 
                    MPI_Vec3,
                    0);
            m_particles.update_velo_vector(velo_buffer);

            //Receive pos vector
            std::vector<Vec3<double> > pos_buffer(size);
            MPI::COMM_WORLD.Bcast(&pos_buffer[0],
                    size,
                    MPI_Vec3,
                    0);
            m_particles.update_pos_vector(pos_buffer);

            // Receive mass vector
            std::vector<double> mass_buffer(size);
            MPI::COMM_WORLD.Bcast(&mass_buffer[0],
                    size,
                    MPI_DOUBLE,
                    0);
            m_particles.update_mass_vector(mass_buffer);

            std::vector<double> radius_buffer(size);
            MPI::COMM_WORLD.Bcast(&radius_buffer[0],
                    size,
                    MPI_DOUBLE,
                    0);

            m_particles.update_radius_vector(radius_buffer);

            std::vector<unsigned long> id_buffer(size);
            MPI::COMM_WORLD.Bcast(&id_buffer[0],
                    size,
                    MPI_DOUBLE,
                    0);
            m_particles.update_id_vector(id_buffer);

            //Calculate chunk start and end
            int start = 0;
            int end;
            for(int i = 0; i < pro_id - 1; i++)
            {
                start += chunk_sizes[i]; 
            }
            end = (start + chunk_sizes[pro_id - 1]);

#ifdef OUTPUT_DEBUG
            std::cout << "start of "<< pro_id <<": "<< start << std::endl;
            std::cout << "end of "<< pro_id <<": "<< end << std::endl;


            //Actual funtion this is all for
            std::cout << pro_id << ": start apply" << std::endl;
            std::cout << "pro_id: "<< pro_id << "Size Velo: " << m_particles.get_velo_vector().size();
            std::cout << "pro_id: "<< pro_id << "Size Pos: " << m_particles.get_pos_vector().size();
            std::cout << "pro_id: "<< pro_id << "Size mass: " << m_particles.get_mass_vector().size() << std::endl;
            for(unsigned long i = start; i < end; i++)
            {
                std::cout << "Pro: " << pro_id  << " " << i << " " <<(m_particles.get_velo_vector()[i]).toString() << std::endl;
                std::cout << "Pro: " << pro_id  << " " << i << " " <<(m_particles.get_pos_vector()[i]).toString() << std::endl;
                std::cout << "Pro: " << pro_id  << " " << i << " " <<(m_particles.get_mass_vector()[i]) << std::endl;
            }
#endif

            unsigned long number_of_collisons = m_particles.detect_collision(start + 2, end);
            
            end = end - number_of_collisons;
            unsigned long new_size = chunk_sizes[pro_id - 1] - number_of_collisons;
            if(pro_id == 1)
            {
                m_particles.apply_gravity_loop(start + 1,end);
                m_particles.move_objects(start + 1 , end);
            }
            else
            {
                m_particles.apply_gravity_loop(start, end);
                m_particles.move_objects(start, end); 
            }


            MPI::COMM_WORLD.Send(&new_size,1,MPI_UNSIGNED_LONG, 0 ,tag);
            MPI::COMM_WORLD.Send(&m_particles.get_velo_vector()[start],new_size,MPI_Vec3,0,tag);
            MPI::COMM_WORLD.Send(&m_particles.get_pos_vector()[start],new_size,MPI_Vec3,0,tag);
            MPI::COMM_WORLD.Send(&m_particles.get_radius_vector()[start],new_size,MPI_DOUBLE,0,tag);
            MPI::COMM_WORLD.Send(&m_particles.get_mass_vector()[start],new_size,MPI_DOUBLE,0,tag);
            MPI::COMM_WORLD.Send(&m_particles.get_id_vector()[start],new_size,MPI_UNSIGNED_LONG,0,tag);

#ifdef OUTPUT_DEBUG 
            std::cout << pro_id << ": iteration done" << std::endl;
#endif
        }
        current_iteration++;
        //Wrtite simulation data to file and in last iteration save last iteration
    }

#ifdef OUTPUT_DEBUG
    std::cout << "all done" << std::endl;
#endif
    //std::cout << pro_id << ": reached end" << std::endl;
}
#endif
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
        m_particles.particle_bubble_sort();
        m_particles.apply_gravity_loop(1);
        m_particles.detect_collision(1);
        m_particles.move_objects(1);
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
 * */
void Simulator::set_up_simulation()
{
    if(m_option_load_from_file)
    {
        std::cout << "loading of file " << m_name_input_file << " done" << std::endl;
        m_particles.load_data_from_file(m_name_input_file, m_number_of_iterations_previous_run);
        m_number_of_iterations_previous_run++;
    }
    else
    {
        //SUN
        m_particles.create_object(Vec3<double>(0,0,0),
                Vec3<double>(0,0,0),
                1.98855 * pow(10, 30),
                696300000);
        //EARTH
        m_particles.create_object(Vec3<double>(AU,0,0),
                Vec3<double>(0,29780,0),
                5.972 * pow(10, 24),
                6371000.0);

        //Generation of random objects
        std::cout << "Generating " << m_number_of_particles << " Objects" << std::endl;

        for(unsigned long i = 0; i < m_number_of_particles - 2 ; i++)
        {
            m_particles.generate_random_object(AU , 20000,5.972 * pow(10, 24) * 6, 6371000 * 15);
        }
        std::cout << "Generating done" << std::endl;
    }

}

/** 
 *  checks if a string is all digits
 * */
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
    if (argc < 2 || argc > 4)
    {
        std::cerr << "Error: invalid number of arguments!" << std::endl;
        usage();
        exit(EXIT_FAILURE);
    }

    if(strcmp(argv[1], "load") == 0)
    {
        if (argc < 4)
        {
            std::cerr << "Error: invalid number of arguments!" << std::endl;
            usage();
            exit(EXIT_FAILURE);
        }

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
    if (strcmp(argv[1], "help") == 0)
    {
        print_help();
        exit(EXIT_SUCCESS);
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

void Simulator::usage()
{
    std::cerr << "Usage: ./Simulation.x [option [file]] [particles] [iterations] [output file]" << std::endl;
    std::cerr << "Operations:" << std::endl;
    std::cerr << "\t./Simualtion.x help" << std::endl;
    std::cerr << "\t./Simualtion.x load <filename> <iterations>" << std::endl;
    std::cerr << "\t./Simualtion.x <particles> <iterations>" << std::endl;
    std::cerr << std::endl;
    std::cerr << "Use './Simulation.x help' for more expansive information" << std::endl;
}

void Simulator::print_help()
{
    std::cout << "Usage: ./Simulation.x [option [file]] [particles] [iterations] [output file]" << std::endl;
    std::cout << std::endl;
    std::cout << "Option | Description" << std::endl;
    std::cout << "-------------------------------------------------------------" << std::endl;
    std::cout << "  help | print this help message" << std::endl;
    std::cout << "  load | specifiy a file to load and continue simulating with" << std::endl;
    std::cout << "<none> | simply specify a number of particles, iterations," << std::endl;
    std::cout << "       | and an output file to run a normal simulation!" << std::endl;
}

