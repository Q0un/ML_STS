#include <iostream>
#include <cassert>

#include "env.h"

int main() {
    Env env;

    while (true) {
        int32_t x;
        std::cin >> x;
        if (x == -1) {
            env.Reset();
            std::cout << 0 << std::endl;
            env.PrintState();
            continue;
        } else if (x == -2) {
            break;
        }
        assert(x >= 0);
        std::cout << env.Step(env.GetActs()[x]) << std::endl;
        env.PrintState();
    }
}
