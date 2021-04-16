#include "mob.h"

// Mob moves

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
            player.take_dmg(mob.deal_dmg(dmg));
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

// Mob

Mob::Mob() {
    hp = cur_move = -1;
    type = Mob_type::NONE;
}

json Mob::get_json() const {
    json res;
    res["max_hp"] = max_hp;
    res["hp"] = hp;
    res["type"] = type;
    res["move"] = cur_move;
    res["effects"] = json::array();
    for (int i = 0; i < (int)Effect::N_EFFECTS; i++) {
        res["effects"].emplace_back(effects[i]);
    }
    return res;
}

void Mob::move(Entity &player) {
    available_moves[cur_move].apply(player, *this);
    for (int i = 0; i < (int)Effect::N_TEMPS; i++) {
        if (effects[i]) {
            effects[i]--;
        }
    }
    cur_move = get_move();
}

int Mob::get_move() const {
    return -1;
}

// Samples

// Jaw Worm

Jaw_Worm::Jaw_Worm() : Mob() {
    int hpl = 40;
    int hpr = 44;
    max_hp = hp = hpl + rnd() % (hpr - hpl + 1);
    type = Mob_type::jaw_worm;
    Mob_moves chomp({ {Mob_movetype::attack, {11, 1}} });
    Mob_moves thrash({ {Mob_movetype::attack, {7, 1}}, {Mob_movetype::defend, {5}} });
    Mob_moves bellow({ {Mob_movetype::defend, {6}}, {Mob_movetype::buff, {(int)Effect::strength, 3}} });
    available_moves = {chomp, thrash, bellow};
    cur_move = rnd() % 3;
}

int Jaw_Worm::get_move() const {
    return rnd() % 3;
}

