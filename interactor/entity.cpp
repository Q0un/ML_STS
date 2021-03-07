#include "entity.h"

Entity::Entity(int max_hp) : max_hp(max_hp), hp(max_hp), def(0) {}

void Entity::give_dmg(int dmg) {
    if (effects[Effect::vulnerable]) {
        dmg += dmg / 2;
    }
    int def_dmg = std::min(def, dmg);
    def -= def_dmg;
    dmg -= def_dmg;
    hp = std::max(0, hp - dmg);
}

void Entity::add_effect(Effect e, int val) {
    effects[e] = val;
}

void Entity::add_def(int val) {
    def += val;
}

bool Entity::dead() const {
    return hp == 0;
}

int Entity::get_hp() const {
    return hp;
}