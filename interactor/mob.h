#pragma once
#include "entity.h"
#include "nlohmann/json.hpp"
using json = nlohmann::json;


enum Mob_type{Test};
enum Mob_movetype{attack, defend, buff, debuff};


class Mob : public Entity {
    int type, cur_move;

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

    void apply(Entity &player, Entity &mob);
};

class Mob_moves {
    std::vector<Mob_move> moves;

public:
    Mob_moves() = default;
    Mob_moves(const std::vector<Mob_move> &moves);

    void apply(Entity &player, Entity &mob);
};

class Sample_Mob {
public:
    int hpl, hpr;
    std::vector<Mob_moves> available_moves;
    std::vector<Effect> effects;

    Sample_Mob() = default;
};

class Jaw_Worm : public Sample_Mob {
    Jaw_Worm();
};

void load_mobs();