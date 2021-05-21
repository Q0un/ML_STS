#include "player.h"

Player::Player(int max_hp) : Entity(max_hp) {}

Player::Player(json &info) : Entity() {
    load(info);
}

json Player::getJson() const {
    json res;
    res["max_hp"] = max_hp;
    res["hp"] = hp;
    res["def"] = def;
    return res;
}

void Player::update() {
    def = 0;

    effects[(int)Effect::Strength] += effects[(int)Effect::Ritual];

    for (int i = 0; i < (int)Effect::N_EFFECTS; i++) {
        if (effects[i]) {
            effects[i]--;
        }
    }
}

void Player::load(json &info) {
    hp = info["hp"];
    max_hp = info["max_hp"];
    def = info["def"];
}

void Player::reset() {
    hp = max_hp;
    def = 0;
}
