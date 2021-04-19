#ifndef STS_PROJECT_CARDS_H
#define STS_PROJECT_CARDS_H

#include <vector>
#include "entity.h"
#include "nlohmann/json.hpp"
using json = nlohmann::json;

enum class CardEffect{Vulnerable, N_CEFFECTS};
enum class CardType{Attack, Skill};

class Card {
    CardType type;
    int dmg, def, cost;
    std::vector<std::pair<CardEffect, int>> effects;

public:
    Card() = default;
    Card(CardType type, int dmg, int def, int cost);
    Card(json &info);

    void addEffect(CardEffect e, int val);
    json getJson() const;
    int getCost() const;
    void use(Entity *player, Entity *mob = nullptr) const;
    void load(json &info);
    CardType getType() const;
};

#endif //STS_PROJECT_CARDS_H
