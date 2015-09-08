//#define GLFW_INCLUDE_GLU
//https://www.youtube.com/watch?v=H20stuPG-To
#include "Util/vec3.hpp"
#include "particle.hpp"
#include "Tests/tester.hpp"
#include "physics.hpp"
#include "simulator.hpp"
#include "Util/macros.hpp"

#include <iostream>
#include <ostream>
#include <vector>

#include <cstdlib>

#include <random>
#include <array>
#include <utility>
#include <stdlib.h>

#include <boost/mpi.hpp>
//#include <mpi.h>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

// global mpi variables
int rank;
int worldsize;

int main(int argc, char **argv)
{
    // analog to MPI_Init and MPI_Finalize
    boost::mpi::environment env(argc, argv);

    // our communicator (contains all processes by default)
    boost::mpi::communicator world;

    DEBUG("Initialized mpi environment!");

    // get this process' rank
    rank = world.rank();

    // get world size
    worldsize = world.size();

    // each process gets its own simulator
    Simulator simulator;

    DEBUG("Created Simulator!");

    // each process processes options, because otherwise
    // we'd have to send them around which would be just
    // as inefficient. get_options takes no time at all...
    simulator.get_options(argc, argv);
    DEBUG("Processed options!");

    // only rank 0 sets up the simulation and then broadcasts it
    // (broadcasting happens in set_up_simulation())
    if (rank == 0)
        simulator.set_up_simulation();
    DEBUG("Set up simulation complete!");

    // wait for set-up to complete
    world.barrier();

    // every process does the simulation
    simulator.simulate();
    DEBUG("Simulation complete!");

	return EXIT_SUCCESS;
}
