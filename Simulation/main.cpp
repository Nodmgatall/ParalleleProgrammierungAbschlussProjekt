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
#include <SDL2/SDL.h>
#include <stdlib.h>

#include <boost/mpi.hpp>
#include <mpi.h>

// global mpi variables
int rank;
int worldsize;

int main(int argc, char **argv)
{
    // analog to MPI_Init and MPI_Finalize
    boost::mpi::environment env(argc, argv);

    // get this process' rank
    rank = MPI::COMM_WORLD.Get_rank();

    // get world size
    worldsize = MPI::COMM_WORLD.Get_size();

    // each process gets its own simulator
    Simulator simulator;

    // each process processes options, because otherwise
    // we'd have to send them around which would be just
    // as inefficient. get_options takes no time at all...
    simulator.get_options(argc, argv);

    // only rank 0 sets up the simulation and then scatters it
    if (rank == 0)
        simulator.set_up_simulation();

    // every process does the simulation
    // TODO: actually parallelize it :D
    simulator.simulate();

	return EXIT_SUCCESS;
}
