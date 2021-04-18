#include "entity.h"

Entity::Entity(int max_hp) : max_hp(max_hp), hp(max_hp), def(0) {
    std::fill(effects, effects + (int)Effect::N_EFFECTS, 0);
}

int Entity::dealDmg(int dmg) const {
    dmg += effects[(int)Effect::strength];
    return dmg;
}

void Entity::takeDmg(int dmg) {
    if (effects[(int)Effect::vulnerable]) {
        dmg += dmg / 2;
    }
    int def_dmg = std::min(def, dmg);
    def -= def_dmg;
    dmg -= def_dmg;
    hp = std::max(0, hp - dmg);
}

void Entity::addEffect(Effect e, int val) {
    effects[(int)e] = val;
}

void Entity::addDef(int val) {
    def += val;
}

bool Entity::dead() const {
    return hp == 0;
}

int Entity::getHp() const {
    return hp;
}

int Entity::getDHp() const {
    return max_hp - hp;
}