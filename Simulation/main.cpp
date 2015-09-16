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

int main(int argc, char **argv)
{
    Simulator simulator;
    simulator.get_options(argc, argv);
    simulator.set_up_simulation();
    simulator.simulate();

	return EXIT_SUCCESS;
}
