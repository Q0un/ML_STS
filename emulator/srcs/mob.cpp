#include "mob.h"

// Mob moves

MobMoveAttack::MobMoveAttack(uint32_t dmg, uint32_t countDmg)
    : Dmg_(dmg)
    , CountDmg_(countDmg)
{
}

void MobMoveAttack::Apply(Entity& player, Entity& mob) {
    for (uint32_t i = 0; i < CountDmg_; i++) {
        player.TakeDmg(mob.DealDmg(Dmg_));
    }
}

MobMoveDefend::MobMoveDefend(uint32_t def)
    : Def_(def)
{
}

void MobMoveDefend::Apply(Entity& player, Entity& mob) {
    mob.AddDef(Def_);
}

MobMoveBuff::MobMoveBuff(Effect effect, uint32_t countEffect)
    : Effect_(effect)
    , CountEffect_(countEffect)
{
}

void MobMoveBuff::Apply(Entity& player, Entity& mob) {
    mob.AddEffect(Effect_, CountEffect_);
}

MobMoveDebuff::MobMoveDebuff(Effect effect, uint32_t countEffect)
    : Effect_(effect)
    , CountEffect_(countEffect)
{
}

void MobMoveDebuff::Apply(Entity& player, Entity& mob) {
    player.AddEffect(Effect_, CountEffect_);
}

MobMoves::MobMoves(uint32_t id, const std::vector<MobMove*>& moves)
    : Id_(id)
    , Moves_(moves)
{
}

void MobMoves::Apply(Entity& player, Entity& mob) {
    for (auto& move : Moves_) {
        move->Apply(player, mob);
    }
}

const std::vector<MobMove*>& MobMoves::GetMoves() const {
    return Moves_;
}

uint32_t MobMoves::GetId() const {
    return Id_;
}

// Mob

Json Mob::ToJson() const {
    Json res;
    res["max_hp"] = MaxHp_;
    res["hp"] = Hp_;
    res["type"] = Type_;
    res["move"] = AvailableMoves_[CurMove_].GetId();
    res["def"] = Def_;
    res["effects"] = Json::array();
    for (size_t i = 0; i < static_cast<size_t>(Effect::N_EFFECTS); i++) {
        res["effects"].emplace_back(Effects_[i]);
    }
    return res;
}

void Mob::Move(Entity& player) {
    Effects_[static_cast<size_t>(Effect::Strength)] += Effects_[static_cast<size_t>(Effect::Ritual)];
    AvailableMoves_[CurMove_].Apply(player, *this);

    for (size_t i = 0; i < static_cast<size_t>(Effect::N_TEMPORARY_EFFECTS); i++) {
        if (Effects_[i]) {
            Effects_[i]--;
        }
    }

    History_.push_back(CurMove_);
    if (History_.size() > 3) {
        History_.pop_front();
    }
    ChooseMove();
}

// Samples

// Jaw Worm

JawWorm::JawWorm() {
    static const uint32_t HP_L = 40;
    static const uint32_t HP_R = 45;
    MaxHp_ = Hp_ = Random::GetInstance()(HP_L, HP_R);
    Type_ = MobType::JawWorm;
    MobMoves chomp(
        1,
        {
            new MobMoveAttack(11, 1)
        }
    );
    MobMoves thrash(
        3,
        {
            new MobMoveAttack(7, 1),
            new MobMoveDefend(5)
        }
    );
    MobMoves bellow(
        2,
        {
            new MobMoveDefend(6),
            new MobMoveBuff(Effect::Strength, 3)
        }
    );
    AvailableMoves_ = {chomp, thrash, bellow};
    ChooseMove();
}

uint32_t JawWorm::ChooseMove() {
    uint32_t mv = 0;
    if (History_.empty()) {
        mv = 0;
    } else if (History_.back() == 2) {
        mv = Random::GetInstance()({25, 30, 0});
    } else if (History_.back() == 0) {
        mv = Random::GetInstance()({0, 30, 45});
    } else if (History_.back() == 1 && History_.size() > 1 && History_[History_.size() - 2] == 1) {
        mv = Random::GetInstance()({25, 0, 45});
    } else {
        mv = Random::GetInstance()({25, 30, 45});
    }
    return CurMove_ = mv;
}

// Cultist

Cultist::Cultist() {
    static const uint32_t HP_L = 48;
    static const uint32_t HP_R = 55;
    MaxHp_ = Hp_ = Random::GetInstance()(HP_L, HP_R);
    Type_ = MobType::Cultist;
    MobMoves incantation(
        3,
        {
            new MobMoveBuff(Effect::Ritual, 3)
        }
    );
    MobMoves darkStrike(
        1,
        {
            new MobMoveAttack(6, 1)
        }
    );
    AvailableMoves_ = {incantation, darkStrike};
    ChooseMove();
}

uint32_t Cultist::ChooseMove() {
    uint32_t mv = 0;
    if (History_.empty()) {
        mv = 0;
    } else {
        mv = 1;
    }
    return CurMove_ = mv;
}

// Red Louse

RedLouse::RedLouse() {
    static const uint32_t HP_L = 10;
    static const uint32_t HP_R = 16;
    MaxHp_ = Hp_ = Random::GetInstance()(HP_L, HP_R);
    Type_ = MobType::RedLouse;

    static const uint32_t CURL_UP_L = 3;
    static const uint32_t CURL_UP_R = 8;
    AddEffect(Effect::CurlUp, Random::GetInstance()(CURL_UP_L, CURL_UP_R));

    static const uint32_t DMG_L = 5;
    static const uint32_t DMG_R = 8;
    MobMoves bite(
        3,
        {
            new MobMoveAttack(Random::GetInstance()(DMG_L, DMG_R), 1)
        }
    );
    MobMoves grow(
        4,
        {
            new MobMoveBuff(Effect::Strength, 3),
        }
    );

    AvailableMoves_ = {bite, grow};
    ChooseMove();
}

uint32_t RedLouse::ChooseMove() {
    uint32_t mv = 0;
    if (History_.size() < 2 || History_.back() != History_[History_.size() - 2]) {
        mv = Random::GetInstance()({75, 25});
    } else {
        if (History_.back() == 0) {
            mv = 1;
        } else {
            mv = 0;
        }
    }
    return CurMove_ = mv;
}

// Green Louse

GreenLouse::GreenLouse() {
    static const uint32_t HP_L = 11;
    static const uint32_t HP_R = 18;
    MaxHp_ = Hp_ = Random::GetInstance()(HP_L, HP_R);
    Type_ = MobType::GreenLouse;

    static const uint32_t CURL_UP_L = 3;
    static const uint32_t CURL_UP_R = 8;
    AddEffect(Effect::CurlUp, Random::GetInstance()(CURL_UP_L, CURL_UP_R));

    static const uint32_t DMG_L = 5;
    static const uint32_t DMG_R = 8;
    MobMoves bite(
        3,
        {
            new MobMoveAttack(Random::GetInstance()(DMG_L, DMG_R), 1)
        }
    );
    MobMoves spitWeb(
        4,
        {
            new MobMoveDebuff(Effect::Weak, 2),
        }
    );

    AvailableMoves_ = {bite, spitWeb};
    ChooseMove();
}

uint32_t GreenLouse::ChooseMove() {
    uint32_t mv = 0;
    if (History_.size() < 2 || History_.back() != History_[History_.size() - 2]) {
        mv = Random::GetInstance()({75, 25});
    } else {
        if (History_.back() == 0) {
            mv = 1;
        } else {
            mv = 0;
        }
    }
    return CurMove_ = mv;
}
