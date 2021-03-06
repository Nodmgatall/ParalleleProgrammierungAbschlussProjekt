#ifndef PHYSICS_HPP
#define PHYSICS_HPP

#include "particle.hpp"
#include "Util/vec3.hpp"

const double G = 0.000000000066742;

static double calculateAccleration(double dist,
						   	double mass_other)
{
	return (G * (mass_other / (dist * dist)));
}

static double __attribute__((unused)) get_time_of_closest_approach(Vec3<double> pos1, Vec3<double> vec1, Vec3<double> pos2, Vec3<double> vec2)
{
    if((vec1 - vec2).getLength() == 0.0)
    {
        return 0.0;
    }
    return -Vec3<double>::dotProduct((pos1 - pos2) ,vec1 - vec2)/ Vec3<double>::dotProduct(vec1 - vec2, vec1 - vec2);
}

static void __attribute__((unused)) apply_gravity(Particle *particle, unsigned long id, double dt)
{
	Vec3<double> distance_vector;
	Vec3<double> accelration_vector;
	Vec3<double> pos_vec_other;
	Vec3<double> pos_vec_curr;
	Vec3<double> normalised_dist_vec;
	unsigned long num_particles = particle->get_pos_vector().size();
	double force;

    // TODO this is where things go wrong in parallel,
    // as the processes don't have the position vector!
    pos_vec_curr = particle->getPosition(id);

	for(unsigned int i = 0; i < id; i++)
	{
        pos_vec_other = particle->getPosition(i);
		distance_vector = pos_vec_other - pos_vec_curr;
		force = calculateAccleration(distance_vector.getLength(),particle->get_mass(i));
		distance_vector.normalise();
		accelration_vector = distance_vector * force / dt;
		// puts("acc: ");
		// accleration_vector.display();
		particle->add_acceleration_vector(id,accelration_vector);
        particle->set_max_velo(std::fmax(accelration_vector.getLength(), particle->get_max_velo()));
	}
	for(unsigned int i = id + 1; i < num_particles; i++)
	{
		pos_vec_other = particle->getPosition(i);
		distance_vector = pos_vec_other - pos_vec_curr;
		force = calculateAccleration(distance_vector.getLength(),particle->get_mass(i));
		distance_vector.normalise();
		accelration_vector = distance_vector * force / dt;
		// puts("acc: ");
		// accleration_vector.display();
		particle->add_acceleration_vector(id,accelration_vector);
        particle->set_max_velo(std::fmax(accelration_vector.getLength(), particle->get_max_velo()));
	}
}
#endif
