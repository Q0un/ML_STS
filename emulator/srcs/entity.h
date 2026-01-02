#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

enum class Effect{
    Vulnerable,
    N_TEMPORARY_EFFECTS,
    Strength,
    Ritual,
    N_EFFECTS
};

class Entity {
public:
    Entity(uint32_t maxHp = 0);

    uint32_t DealDmg(uint32_t dmg) const;
    void TakeDmg(uint32_t dmg);
    void AddEffect(Effect e, uint32_t val);
    void AddDef(uint32_t val);
    bool Dead() const;
    uint32_t GetHp() const;
    uint32_t GetDHp() const;
    uint32_t GetDef() const;

protected:
    uint32_t MaxHp_ = 0;
    uint32_t Hp_ = 0;
    uint32_t Def_ = 0;
    std::array<uint32_t, static_cast<size_t>(Effect::N_EFFECTS)> Effects_;
};
