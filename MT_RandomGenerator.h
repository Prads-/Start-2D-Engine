#ifndef MT_RANDOM_GENERATOR_H
#define MT_RANDOM_GENERATOR_H

#include <random>

class MT_RandomGenerator {
private:
	static std::mt19937 randomGenerator;

public:
	static void seed(long long s);
	static unsigned int randomNumber();
};

#endif