#include "env.h"
#include <iostream>
#include <cassert>
using namespace std;

int main() {
    Env env;

    while (true) {
        int x;
        cin >> x;
        if (x == -1) {
            env.reset();
            std::cout << 0 << std::endl;
            env.printState();
            continue;
        } else if (x == -2) {
            break;
        }
        std::cout << env.step(env.getActs()[x]) << std::endl;
        env.printState();
    }
}
