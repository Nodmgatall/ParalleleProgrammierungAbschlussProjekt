#include <cstdlib>
#include <iostream>
#include <ostream>
#include <random>
#include <array>
#include <utility>

#include "vec3.hpp"
#include "particle.hpp"
int main(int argc, char** argv)
{
	Vec3<double> test_vector(5,5,5);
	Vec3<double> lol_2(5,5,5);
	Vec3<double> rofl = test_vector + lol_2;
	
	Particle particle;

	particle.generateRandomParticle(
		std::make_pair<double,double>(500000.0,100000000.0),
		std::make_pair<double,double>(1.0,50.0),
		std::make_pair<double,double>(1.0,5000),
		std::make_pair<double,double>(1.0,5000));

	test_vector.display();
	rofl.display();
	particle.printParticle(0);
	std::cout << rofl.getLength() << std::endl << std::endl;
	return EXIT_SUCCESS;
}