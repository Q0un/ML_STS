#include "env.h"
#include <iostream>
#include <cassert>
using namespace std;

int main() {
    Env env;
    env.reset();
    env.startFight();

    while (env.getGamestate() == State::fight) {
        env.step(env.sampleAct());
    }
}
