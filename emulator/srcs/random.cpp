#include <cassert>

#include "random.h"

Random* Random::InstancePtr_ = nullptr;

Random::Random(unsigned int seed) : RndGenerator_(seed) {}

Random& Random::GetInstance() {
    if (InstancePtr_ == nullptr) {
        InstancePtr_ = new Random(std::random_device()());
    }
    return *InstancePtr_;
}

uint32_t Random::operator()(uint32_t n) {
    return RndGenerator_() % n;
}

uint32_t Random::operator()(uint32_t from, uint32_t to) {
    return from + RndGenerator_() % (to - from);
}

uint32_t Random::operator()(const std::vector<uint32_t>& ps) {
    uint32_t sum = 0;
    for (uint32_t i : ps) {
        sum += i;
    }
    uint32_t x = RndGenerator_() % sum;
    uint32_t prf = 0;
    for (size_t i = 0; i < ps.size(); i++) {
        prf += ps[i];
        if (x < prf) {
            return i;
        }
    }
    assert(0);
}

std::mt19937& Random::GetRndGenerator() {
    return RndGenerator_;
}
