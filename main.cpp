#include "interactor/env.h"
#include <iostream>
#include <cassert>
using namespace std;

int main() {
    Env env;
    env.reset();
    env.start_fight();

    while (env.get_gamestate() == State::fight) {
        env.step(env.sample_act());
    }
}
