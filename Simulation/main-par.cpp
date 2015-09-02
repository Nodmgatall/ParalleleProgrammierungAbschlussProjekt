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

// mpi.h contains C bindings as well as C++ bindings
#include <mpi.h>

// our global variables saving MPI world size and processor rank
int rank;
int worldsize;

int main(int argc, char **argv)
{
    // call to MPI::Init() is mandatory
    MPI::Init(argc, argv);

    // query for our rank and world's size
    rank = MPI::COMM_WORLD.Get_rank();
    worldsize = MPI::COMM_WORLD.Get_size();

    // every CPU has it's own Simulator
    Simulator simulator;

    // only CPU #0 processes options and sets up the simulation
    if (rank == 0)
    {
        simulator.get_options(argc, argv);
        simulator.set_up_simulation();
    }

    // TODO parallelize like nobody's business
    simulator.simulate();

    // MPI::Finalize() is mandatory at the end of MPI-Programs
    MPI::Finalize();

	return EXIT_SUCCESS;
}
