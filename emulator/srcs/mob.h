#pragma once

#include <queue>

#include "entity.h"
#include "json.h"
#include "random.h"

enum class MobType{
    JawWorm,
    Cultist,
    None
};

class MobMove {
public:
    MobMove() = default;

    virtual void Apply(Entity& player, Entity& mob) = 0;
};

class MobMoveAttack : public MobMove {
public:
    MobMoveAttack(uint32_t dmg, uint32_t countDmg);

    void Apply(Entity& player, Entity& mob) override;

private:
    uint32_t Dmg_ = 0;
    uint32_t CountDmg_ = 0;
};

class MobMoveDefend : public MobMove {
public:
    MobMoveDefend(uint32_t def);

    void Apply(Entity& player, Entity& mob) override;

private:
    uint32_t Def_ = 0;
};

class MobMoveBuff : public MobMove {
public:
    MobMoveBuff(Effect effect, uint32_t countEffect);

    void Apply(Entity& player, Entity& mob) override;

private:
    Effect Effect_ = Effect::N_EFFECTS;
    uint32_t CountEffect_ = 0;
};

class MobMoveDebuff : public MobMove {
public:
    MobMoveDebuff(Effect effect, uint32_t countEffect);

    void Apply(Entity& player, Entity& mob) override;

private:
    Effect Effect_ = Effect::N_EFFECTS;
    uint32_t CountEffect_ = 0;
};

class MobMoves {
public:
    MobMoves(uint32_t id, const std::vector<MobMove*>& moves);

    void Apply(Entity& player, Entity& mob);
    const std::vector<MobMove*>& GetMoves() const;
    uint32_t GetId() const;

private:
    uint32_t Id_;
    std::vector<MobMove*> Moves_;
};

class Mob : public Entity {
public:
    Mob() = default;

    void Move(Entity& player);
    Json ToJson() const;

protected:
    virtual uint32_t ChooseMove() = 0;

protected:
    MobType Type_ = MobType::None;
    uint32_t CurMove_ = 0;
    std::deque<uint32_t> History_;
    std::vector<MobMoves> AvailableMoves_;
};

class JawWorm : public Mob {
public:
    JawWorm();

protected:
    uint32_t ChooseMove() override;
};

class Cultist : public Mob {
public:
    Cultist();

protected:
    uint32_t ChooseMove() override;
};
