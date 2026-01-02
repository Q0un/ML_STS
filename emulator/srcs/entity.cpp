#include "entity.h"

Entity::Entity(uint32_t maxHp)
    : MaxHp_(maxHp)
    , Hp_(maxHp)
{
}

uint32_t Entity::DealDmg(uint32_t dmg) const {
    dmg += Effects_[static_cast<size_t>(Effect::Strength)];
    return dmg;
}

void Entity::TakeDmg(uint32_t dmg) {
    if (Effects_[static_cast<size_t>(Effect::Vulnerable)]) {
        dmg += dmg / 2;
    }
    uint32_t defDmg = std::min(Def_, dmg);
    Def_ -= defDmg;
    dmg -= defDmg;
    if (dmg > Hp_) {
        Hp_ = 0;
    } else {
        Hp_ -= dmg;
    }
}

void Entity::AddEffect(Effect e, uint32_t val) {
    Effects_[static_cast<size_t>(e)] = val;
}

void Entity::AddDef(uint32_t val) {
    Def_ += val;
}

bool Entity::Dead() const {
    return Hp_ == 0;
}

uint32_t Entity::GetHp() const {
    return Hp_;
}

uint32_t Entity::GetDHp() const {
    return MaxHp_ - Hp_;
}

uint32_t Entity::GetDef() const {
    return Def_;
}