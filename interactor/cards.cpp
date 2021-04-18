#include "cards.h"

Card::Card(Card_type type, int dmg, int def, int cost) : type(type), dmg(dmg), def(def), cost(cost) {}

Card::Card(json &info) {
    load(info);
}

void Card::addEffect(Card_effect e, int val) {
    effects.emplace_back(e, val);
}

json Card::getJson() const {
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

int Card::getCost() const {
    return cost;
}

void Card::use(Entity *player, Entity *mob) const {
    if (dmg) {
        mob->takeDmg(player->dealDmg(dmg));
    }
    if (def) {
        player->addDef(def);
    }
    if (mob) {
        for (auto &e : effects) {
            mob->addEffect((Effect) e.first, e.second);
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

Card_type Card::getType() const {
    return type;
}