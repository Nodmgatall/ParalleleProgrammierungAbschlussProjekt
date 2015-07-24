#define GLFW_INCLUDE_GLU
//https://www.youtube.com/watch?v=H20stuPG-To
#include "Util/vec3.hpp"
#include "particle.hpp"
#include "Tests/tester.hpp"
#include "physics.hpp"

#include <iostream>
#include <ostream>
#include <vector>

#include <cstdlib>

#include <random>
#include <array>
#include <utility>
#include <SDL2/SDL.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
    if(argc < 3)
    {
        std::cout << "Not enough arguments " << argc << "/3" << std::endl;
    }
    
    double dt = 0.000024;
    Particle particle;
    
    unsigned long number_of_objects = strtoul(argv[1],NULL,0);
    unsigned long number_of_iterations = strtoul(argv[2],NULL,0);
    
    particle.createParticle(Vec3<double>(0,0,0),
                             Vec3<double>(0,0,0),
                             1.98855 * pow(10, 30),
                             10.0);
    particle.createParticle(Vec3<double>(149598261000,0,0),
                             Vec3<double>(0,29780,0),
                             5.972 * pow(10, 24),
                             10.0);

    std::cout << "Generating " << number_of_objects << " Objects" << std::endl;
    for(unsigned long i = 0; i < number_of_objects; i++)
    {
        
        particle.generateRandomParticle(149598261000 * 20, 40000,5.972 * pow(10, 24) * 6, 100000000000000000);
    }
    std::cout << "Generating done" << std::endl;
    
    for(unsigned long i = number_of_iterations; i < number_of_iterations; i++)
    {
        particle.writePositionToFile(argv[3]);
        std::cout << "Iteration " << i << " of" << number_of_iterations << std::endl;
        for (unsigned long i = 0; i < number_of_objects; i++)
        {
            applyGravity(particle,i+1,dt);
            particle.move_Object(i+1,dt);

        }
    }
    //first_test();
    //test_grav();

	return EXIT_SUCCESS;
}
