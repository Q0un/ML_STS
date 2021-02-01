#pragma once
#include "entity.h"
#include "nlohmann/json.hpp"
using namespace std;
using json = nlohmann::json;

/*
enum Mob_type{Test};
enum Mob_movetype{attack, defend, buff, debuff};
*/

class Mob : public Entity {
    int dmg;

public:
    Mob() = default;
    // Mob(Mob_type type);
    Mob(int hp, int dmg);
    Mob(json &info);

    json get_json() const;
    void move(Entity &player);
    void load(json &info);
};

/*
class Mob_move {
    Mob_movetype type;
    int dmg, count_dmg, def;
    Effect effect;
    int count_effect;

public:
    Mob_move() = default;
    Mob_move(Mob_movetype type, const vector<int> &args);
};

class Sample_Mob {
public:
    int hpl, hpr;
    vector<Mob_move> available_moves;
    vector<Effect> effects;

    Sample_Mob() = default;
};

void load_mobs();
*/