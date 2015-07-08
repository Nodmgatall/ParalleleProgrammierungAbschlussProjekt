#include "particle.hpp"
#include "vec3.hpp"
#include "tester.hpp"
#include "physics.hpp"
#include <iostream>
#include <assert.h>
#include <cmath>

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
    double random_x_pos;
    for(int i = 0; i < 10; i++)
    {
    random_x_pos = die_distribution(random_device);
    std::cout << random_x_pos << std::endl;
    }
}

void test_grav()
{
    Particle particle;
    particle. createParticle(Vec3<double>(0,0,0),
                             Vec3<double>(0,0,0),
                             1.98855 * pow(10, 30),
                             10.0);
    particle. createParticle(Vec3<double>(149598261000,0,0),
                             Vec3<double>(0,297800,0),
                             5.972 * pow(10, 24),
                             10.0);

    unsigned long i = 0;
    Vec3<double> start = Vec3<double>(0,149600000000,0);
    Vec3<double> end;
    unsigned long id = 1;
     while(i< 1000)
    //while(i < 6)
    //while(particle.getPostion(id).getLength() > 10)
    {
        if(i % 1 == 0)
        {
            // particle.printParticle(0);
            particle.printParticle(1);

        }
        applyGravity(particle, id, 0.006);
         particle.move_Object(id,1);
      //  particle.printParticle(id);
       // particle.getPostion(id).display();
        i++;
    }
    end = particle.getPostion(id);

    puts("start:");
    start.display();
    puts("end:");
    start.display();



}
