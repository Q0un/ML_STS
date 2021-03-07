#pragma once
#include "entity.h"
#include "nlohmann/json.hpp"
using json = nlohmann::json;


enum Mob_type{Test};
enum Mob_movetype{attack, defend, buff, debuff};


class Mob : public Entity {
    int dmg;

public:
    Mob() = default;
    Mob(Mob_type type);
    Mob(json &info);

    json get_json() const;
    void move(Entity &player);
    void load(json &info);
};


class Mob_move {
    Mob_movetype type;
    int dmg, count_dmg, def;
    Effect effect;
    int count_effect;

public:
    Mob_move() = default;
    Mob_move(Mob_movetype type, const std::vector<int> &args);
};


class Sample_Mob {
public:
    int hpl, hpr;
    std::vector<Mob_move> available_moves;
    std::vector<Effect> effects;

    Sample_Mob() = default;
};
