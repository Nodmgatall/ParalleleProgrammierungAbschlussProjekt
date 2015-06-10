#include "particle.hpp"
#include "vec3.hpp"
#include "tester.hpp"
#include <iostream>
#include <assert.h>

#include <random>
#include <array>
#include <utility>

void first_test() {
    Particle particles;
    //int i;

    std::pair<double,double> range_position (0.0,100000.0);
    std::pair<double,double> range_velocity (1.0,50.0);
    std::pair<double,double> range_mass (1.0,5000.0);
    std::pair<double,double> range_radius (1.0,5000.0);

    //for (i = 0; i < 30; i++) {
        particles.generateRandomParticle(
            range_position,
            range_velocity,
            range_mass,
            range_radius);
    //}

    assert(particles.getNumberOfParticles() == 1);
    
    //particles.remove(15);

    //assert(particles.getNumberOfParticles() == 29);
}

void vectorInitTest()
{
    Vec3<double> test_vector(5,5,5);
    Vec3<double> lol_2(5,5,5);
    Vec3<double> rofl = test_vector + lol_2;
    
    Particle particle;

    test_vector.display();
    rofl.display();
    std::cout << rofl.getLength() << std::endl;
    std::cout << "lol" << std::endl;
} 

void randomGeneratorTest()
{     
    std::random_device random_device;
    std::uniform_real_distribution<double> die_distribution{1.0,500.0};

    for(int i = 0; i < 10; i++)
    {
    double random_x_pos = die_distribution(random_device);
    std::cout << random_x_pos << std::endl;
    }
}
