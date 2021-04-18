#ifndef STS_PROJECT_MOB_H
#define STS_PROJECT_MOB_H

#include "entity.h"
#include "random.h"
#include <queue>
#include "nlohmann/json.hpp"
using json = nlohmann::json;

enum class MobType{JawWorm, None};
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
};

class MobMoves {
    std::vector<MobMove> moves;

public:
    MobMoves() = default;
    MobMoves(const std::vector<MobMove> &moves);

    void apply(Entity &player, Entity &mob);
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

protected:
    virtual int getMove();
};

class JawWorm : public Mob {
public:
    JawWorm();

protected:
    int getMove() override;
};

#endif //STS_PROJECT_MOB_H
