#include "env.h"
#include <iostream>
#include <cassert>
using namespace std;

int main() {
    Env env;
    env.reset();
    env.printState();

    while (env.getGamestate() != State::Lose && env.getGamestate() != State::Win) {
        env.step(env.sampleAct());
        env.printState();
    }
}
