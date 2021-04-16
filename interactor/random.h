#ifndef STS_PROJECT_RANDOM_H
#define STS_PROJECT_RANDOM_H

#include <random>
#include <vector>

#define rnd std::mt19937(std::random_device()())

int get_random(const std::vector<int> &ps);

#endif //STS_PROJECT_RANDOM_H