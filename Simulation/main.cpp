#define GLFW_INCLUDE_GLU
//https://www.youtube.com/watch?v=H20stuPG-To
#include "Util/vec3.hpp"
#include "particle.hpp"
#include "Tests/tester.hpp"
#include "physics.hpp"
#include "simulator.hpp"
#include <iostream>
#include <ostream>
#include <vector>

#include <cstdlib>

#include <random>
#include <array>
#include <utility>
#include <stdlib.h>

#ifdef PARALLEL_BUILD
    #include <mpi.h>
    MPI::Datatype MPI_Vec3;
#endif

#ifdef PARALLEL_BUILD
int main(int argc, char **argv)
{
    MPI::Init();
    int pro_id = MPI::COMM_WORLD.Get_rank();
    Simulator simulator;
    if(pro_id == 0)
    {
        simulator.get_options(argc, argv);
        simulator.set_up_simulation();
        simulator.simulate_parallel();
    }
    else
    {
        simulator.simulate_parallel();
    }
    MPI::Finalize();

    return EXIT_SUCCESS;
}
#else
int main(int argc, char**argv)
{
    Simulator simulator;
    simulator.get_options(argc, argv);
    simulator.set_up_simulation();
    simulator.simulate();

    return EXIT_SUCCESS;
}
#endif
