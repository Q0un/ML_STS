#include "mob.h"
#include "random.h"

// Sample Mobs

std::vector<Sample_Mob> mob_samples;

Jaw_Worm::Jaw_Worm() : Sample_Mob() {
    hpl = 40;
    hpr = 44;
    Mob_moves chomp({ {Mob_movetype::attack, {11, 1}} });
    Mob_moves thrash({ {Mob_movetype::attack, {7, 1}}, {Mob_movetype::defend, {5}} });
    Mob_moves bellow({ {Mob_movetype::defend, {6}}, {Mob_movetype::buff, {Effect::strength, {3}}} });
    available_moves = {chomp, thrash, bellow};

}

// Mob

Mob::Mob(Mob_type type) : Entity() {
    hp = max_hp = mob_samples[type].hpl + rnd() % (mob_samples[type].hpr - mob_samples[type].hpl + 1);
}

Mob::Mob(json &info) : Entity() {
    load(info);
}

json Mob::get_json() const {
    json res;
    res["max_hp"] = max_hp;
    res["hp"] = hp;
    res["type"] = type;
    res["move"] = cur_move;
    res["effects"] = json::array();
    for (int i = 0; i < N_EFFECTS; i++) {
        res["effects"].emplace_back(effects[i]);
    }
    return res;
}

void Mob::move(Entity &player) {
    mob_samples[type].available_moves[cur_move].apply(player, *this);
    for (int i = 0; i < N_TEMPS; i++) {
        if (effects[i]) {
            effects[i]--;
        }
    }
}

void Mob::load(json &info) {
    max_hp = info["max_hp"];
    hp = info["hp"];
    type = info["type"];
    cur_move = info["move"];
    for (int i = 0; i < N_EFFECTS; i++) {
        effects[i] = info["effects"][i];
    }
}

// Mob_move

Mob_move::Mob_move(Mob_movetype type, const std::vector<int> &args) : type(type) {
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

Mob_moves::Mob_moves(const std::vector<Mob_move> &moves) : moves(moves) {}

void Mob_move::apply(Entity &player, Entity &mob) {
    if (type == Mob_movetype::attack) {
        for (int i = 0; i < count_dmg; i++) {
            player.give_dmg(dmg);
        }
    } else if (type == Mob_movetype::defend) {
        mob.add_def(def);
    } else if (type == Mob_movetype::buff) {
        mob.add_effect(effect, count_effect);
    } else if (type == Mob_movetype::debuff) {
        player.add_effect(effect, count_effect);
    }
}

void Mob_moves::apply(Entity &player, Entity &mob) {
    for (auto &move : moves) {
        move.apply(player, mob);
    }
}