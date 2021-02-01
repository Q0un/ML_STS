#include "mob.h"

/*
vector<Sample_Mob> mob_samples;

Mob::Mob(Mob_type type) : Entity() {
    hp = max_hp = mob_samples[type].hpl + rnd();
}
*/

Mob::Mob(int hp, int dmg) : Entity(hp), dmg(dmg) {}

Mob::Mob(json &info) : Entity() {
    load(info);
}

json Mob::get_json() const {
    json res;
    res["max_hp"] = max_hp;
    res["hp"] = hp;
    res["dmg"] = dmg;
    res["effects"] = json::array();
    for (int i = 0; i < N_EFFECTS; i++) {
        res["effects"].emplace_back(effects[i]);
    }
    return res;
}

void Mob::move(Entity &player) {
    player.give_dmg(dmg);
    for (int i = 0; i < N_EFFECTS; i++) {
        if (effects[i]) {
            effects[i]--;
        }
    }
}

void Mob::load(json &info) {
    max_hp = info["max_hp"];
    hp = info["hp"];
    dmg = info["dmg"];
    for (int i = 0; i < N_EFFECTS; i++) {
        effects[i] = info["effects"][i];
    }
}

/*
Mob_move::Mob_move(Mob_movetype type, const vector<int> &args) : type(type) {
    if (type == Mob_movetype::attack) {
        dmg = args[0];
        count_dmg = args[1];
    } else if (type == Mob_movetype::defend) {
        def = args[0];
    } else if (type == Mob_movetype::buff) {
        effect = (Effect)args[0];
        count_effect = args[1];
    } else if (type == Mob_movetype::debuff) {
        effect = (Effect)args[0];
        count_effect = args[1];
    }
}
*/
