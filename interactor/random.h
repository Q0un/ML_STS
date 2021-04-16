#ifndef STS_PROJECT_RANDOM_H
#define STS_PROJECT_RANDOM_H

#include <random>

#define rnd std::mt19937(std::random_device()())

#endif //STS_PROJECT_RANDOM_H