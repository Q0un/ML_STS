#include "random.h"

int getRandom(const std::vector<int> &ps) {
    int sum = 0;
    for (int i : ps) {
        sum += i;
    }
    int x = rnd() % sum;
    int prf = 0;
    for (int i = 0; i < ps.size(); i++) {
        prf += ps[i];
        if (x < prf) {
            return i;
        }
    }
    return -1;
}