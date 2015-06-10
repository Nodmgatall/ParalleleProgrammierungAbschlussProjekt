#include "particle.hpp"
#include "vec3.hpp"
#include "tester.hpp"
#include <iostream>
#include <assert.h>

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
