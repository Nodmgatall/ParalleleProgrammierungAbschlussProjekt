#ifndef RANDOM_GENERATOR_HPP
#define RANDOM_GENERATOR_HPP

#include <utility> 
#include <random>
#include "vec3.hpp"

	template<typename T> 
	T generateRandomNumber(T range_start, T range_end)
	{
		double random_number;
		std::random_device random_device;
		std::uniform_real_distribution<double> distribution{range_start,range_end};

		random_number = distribution(random_device);

		return random_number;
	}

	template<typename T> 
	Vec3<T> generateRandomVec3(T length_min, T length_max)
	{
		double final_length = generateRandomNumber<T>(length_min,length_max);
		Vec3<T> new_vector(
			generateRandomNumber<T>(-10, 10),
			generateRandomNumber<T>(-10, 10),
			generateRandomNumber<T>(-10, 10)
			);
		new_vector *= (final_length/new_vector.getLength());
		return new_vector;
	}
#endif /* RANDOM_GENERATOR_HPP */