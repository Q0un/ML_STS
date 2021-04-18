#include "mob.h"

// Mob moves

MobMove::MobMove(MobMoveType type, const std::vector<int> &args) : type(type) {
    if (type == MobMoveType::Attack) {
        dmg = args[0];
        count_dmg = args[1];
    } else if (type == MobMoveType::Defend) {
        def = args[0];
    } else if (type == MobMoveType::Buff) {
        effect = (Effect)args[0];
        count_effect = args[1];
    } else if (type == MobMoveType::Debuff) {
        effect = (Effect)args[0];
        count_effect = args[1];
    }
}

MobMoves::MobMoves(const std::vector<MobMove> &moves) : moves(moves) {}

void MobMove::apply(Entity &player, Entity &mob) {
    if (type == MobMoveType::Attack) {
        for (int i = 0; i < count_dmg; i++) {
            player.takeDmg(mob.dealDmg(dmg));
        }
    } else if (type == MobMoveType::Defend) {
        mob.addDef(def);
    } else if (type == MobMoveType::Buff) {
        mob.addEffect(effect, count_effect);
    } else if (type == MobMoveType::Debuff) {
        player.addEffect(effect, count_effect);
    }
}

void MobMoves::apply(Entity &player, Entity &mob) {
    for (auto &move : moves) {
        move.apply(player, mob);
    }
}

// Mob

Mob::Mob() {
    hp = cur_move = -1;
    type = MobType::None;
}

json Mob::getJson() const {
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
    getMove();
}

int Mob::getMove() {
    assert(0);
    return cur_move = -1;
}

// Samples

// Jaw Worm

JawWorm::JawWorm() : Mob() {
    int hpl = 40;
    int hpr = 44;
    max_hp = hp = hpl + rnd() % (hpr - hpl + 1);
    type = MobType::JawWorm;
    MobMoves chomp({ {MobMoveType::Attack, {11, 1}} });
    MobMoves thrash({ {MobMoveType::Attack, {7, 1}}, {MobMoveType::Defend, {5}} });
    MobMoves bellow({ {MobMoveType::Defend, {6}}, {MobMoveType::Buff, {(int)Effect::Strength, 3}} });
    available_moves = {chomp, thrash, bellow};
    getMove();
}

int JawWorm::getMove() {
    int mv = -1;
    if (history.empty()) {
        mv = 0;
    } else if (history.back() == 2) {
        mv = getRandom({25, 30, 0});
    } else if (history.back() == 0) {
        mv = getRandom({0, 30, 45});
    } else if (history.back() == 1 && history.size() > 1 && *next(history.rbegin()) == 1) {
        mv = getRandom({25, 0, 45});
    } else {
        mv = getRandom({25, 30, 45});
    }
    return cur_move = mv;
}

