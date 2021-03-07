#include "cards.h"

Card::Card(Card_type type, int dmg, int def, int cost) : type(type), dmg(dmg), def(def), cost(cost) {}

Card::Card(json &info) {
    load(info);
}

void Card::add_effect(Card_effect e, int val) {
    effects.emplace_back(e, val);
}

json Card::get_json() const {
    json res;
    res["dmg"] = dmg;
    res["def"] = def;
    res["cost"] = cost;
    res["effects"] = json::array();
    for (auto &e : effects) {
        res["effects"].emplace_back(std::make_pair(e.first, e.second));
    }
    return res;
}

int Card::get_cost() const {
    return cost;
}

void Card::use(Entity *player, Entity *mob) const {
    if (dmg) {
        mob->give_dmg(dmg);
    }
    if (def) {
        player->add_def(def);
    }
    if (mob) {
        for (auto &e : effects) {
            mob->add_effect((Effect) e.first, e.second);
        }
    }
}

void Card::load(json &info) {
    dmg = info["dmg"];
    def = info["def"];
    cost = info["cost"];
    effects.clear();
    for (auto &e : info["effects"]) {
        effects.emplace_back(e[0], e[1]);
    }
}

Card_type Card::get_type() const {
    return type;
}