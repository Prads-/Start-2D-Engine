#include "MT_RandomGenerator.h"

std::mt19937 MT_RandomGenerator::randomGenerator;

void MT_RandomGenerator::seed(long long s) {
	randomGenerator.seed(s);
}

unsigned int MT_RandomGenerator::randomNumber() {
	return randomGenerator();
}