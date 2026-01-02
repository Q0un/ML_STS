#pragma once

#include <random>
#include <vector>

class Random {
public:
    Random(unsigned int seed);

    static Random& GetInstance();

    uint32_t operator()(uint32_t n);
    uint32_t operator()(uint32_t from, uint32_t to);
    uint32_t operator()(const std::vector<uint32_t>& ps);

    std::mt19937& GetRndGenerator();
private:
    std::mt19937 RndGenerator_;

    static Random* InstancePtr_;
};
