#include "entity.h"

Entity::Entity(int max_hp) : max_hp(max_hp), hp(max_hp), def(0) {
    std::fill(effects, effects + (int)Effect::N_EFFECTS, 0);
}

int Entity::deal_dmg(int dmg) const {
    dmg += effects[(int)Effect::strength];
    return dmg;
}

void Entity::take_dmg(int dmg) {
    if (effects[(int)Effect::vulnerable]) {
        dmg += dmg / 2;
    }
    int def_dmg = std::min(def, dmg);
    def -= def_dmg;
    dmg -= def_dmg;
    hp = std::max(0, hp - dmg);
}

void Entity::add_effect(Effect e, int val) {
    effects[(int)e] = val;
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