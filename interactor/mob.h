#ifndef STS_PROJECT_MOB_H
#define STS_PROJECT_MOB_H

#include "entity.h"
#include "random.h"
#include <queue>
#include "nlohmann/json.hpp"
using json = nlohmann::json;

enum class MobType{JawWorm, Cultist, None};
enum class MobMoveType{Attack, Defend, Buff, Debuff};

class MobMove {
    MobMoveType type;
    int dmg, count_dmg, def;
    Effect effect;
    int count_effect;

public:
    MobMove() = default;
    MobMove(MobMoveType type, const std::vector<int> &args);

    void apply(Entity &player, Entity &mob);
    int getDmg(const Entity &mob) const;
};

class MobMoves {
    int id;
    std::vector<MobMove> moves;

public:
    MobMoves() = default;
    MobMoves(int id, const std::vector<MobMove> &moves);

    void apply(Entity &player, Entity &mob);
    const std::vector<MobMove>& getMoves() const;
    int getId() const;
};

class Mob : public Entity{
protected:
    MobType type;
    int cur_move;
    std::deque<int> history;
    std::vector<MobMoves> available_moves;

public:
    Mob();

    json getJson() const;
    void move(Entity &player);
    int getDmg() const;

protected:
    virtual int chooseMove();
};

class JawWorm : public Mob {
public:
    JawWorm();

protected:
    int chooseMove() override;
};

class Cultist : public Mob {
public:
    Cultist();

protected:
    int chooseMove() override;
};

#endif //STS_PROJECT_MOB_H
