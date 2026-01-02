#pragma once

#include <vector>

#include "entity.h"
#include "json.h"

enum class CardEffect{
    Vulnerable
};
enum class CardType{
    Attack,
    Skill
};

class Card {
public:
    Card(CardType type, uint32_t dmg, uint32_t def, uint32_t cost);

    void AddEffect(CardEffect e, uint32_t val);
    uint32_t GetCost() const;
    void Use(Entity* player, Entity* mob = nullptr) const;
    CardType GetType() const;
    
    static Card FromJson(const Json& info);
    Json ToJson() const;

private:
    CardType Type_;
    uint32_t Dmg_;
    uint32_t Def_;
    uint32_t Cost_;
    std::vector<std::pair<CardEffect, int>> Effects_;
};
