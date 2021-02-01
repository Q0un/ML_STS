#pragma once
#include <vector>
#include "entity.h"
#include "nlohmann/json.hpp"
using namespace std;
using json = nlohmann::json;

enum Card_effect{Cvulnerable, N_CEFFECTS};
enum Card_type{attack, skill};

class Card {
    Card_type type;
    int dmg, def, cost;
    vector<pair<Card_effect, int>> effects;

public:
    Card() = default;
    Card(Card_type type, int dmg, int def, int cost);
    Card(json &info);

    void add_effect(Card_effect e, int val);
    json get_json() const;
    int get_cost() const;
    void use(Entity *player, Entity *mob = nullptr) const;
    void load(json &info);
    Card_type get_type() const;
};
