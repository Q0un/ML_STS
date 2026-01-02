#include "cards.h"

namespace {
    const std::vector<std::string> CARD_TYPE_STR = {
        "ATTACK",
        "SKILL"
    };
}

Card::Card(
    CardType type,
    uint32_t dmg,
    uint32_t def,
    uint32_t cost
)
    : Type_(type)
    , Dmg_(dmg)
    , Def_(def)
    , Cost_(cost)
{
}

void Card::AddEffect(CardEffect e, uint32_t val) {
    Effects_.emplace_back(e, val);
}

uint32_t Card::GetCost() const {
    return Cost_;
}

void Card::Use(Entity* player, Entity* mob) const {
    if (Dmg_ > 0) {
        mob->TakeDmg(player->DealDmg(Dmg_));
    }
    if (Def_ > 0) {
        player->AddDef(Def_);
    }
    if (mob != nullptr) {
        for (const auto& e : Effects_) {
            mob->AddEffect(static_cast<Effect>(e.first), e.second);
        }
    }
}

CardType Card::GetType() const {
    return Type_;
}

Card Card::FromJson(const Json& info) {
    CardType type = static_cast<CardType>(std::find(CARD_TYPE_STR.begin(), CARD_TYPE_STR.end(), info["type"]) - CARD_TYPE_STR.begin());
    Card card(type, info["dmg"], info["def"], info["cost"]);
    for (const auto& e : info["effects"]) {
        card.AddEffect(e[0], e[1]);
    }
    return card;
}

Json Card::ToJson() const {
    Json res;
    res["type"] = CARD_TYPE_STR[static_cast<size_t>(Type_)];
    res["dmg"] = Dmg_;
    res["def"] = Def_;
    res["cost"] = Cost_;
    res["effects"] = Json::array();
    for (const auto& e : Effects_) {
        res["effects"].emplace_back(e);
    }
    return res;
}