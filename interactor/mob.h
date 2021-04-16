#ifndef STS_PROJECT_MOB_H
#define STS_PROJECT_MOB_H

#include "entity.h"
#include "random.h"
#include "nlohmann/json.hpp"
using json = nlohmann::json;

enum class Mob_type{jaw_worm, NONE};
enum class Mob_movetype{attack, defend, buff, debuff};

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

class Mob : public Entity{
protected:
    Mob_type type;
    int cur_move;
    std::vector<Mob_moves> available_moves;

public:
    Mob();

    json get_json() const;
    void move(Entity &player);

protected:
    virtual int get_move() const;
};

class Jaw_Worm : public Mob {
public:
    Jaw_Worm();

protected:
    int get_move() const override;
};

#endif //STS_PROJECT_MOB_H
