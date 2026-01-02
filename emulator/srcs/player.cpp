#include "player.h"

Player::Player(uint32_t maxHp)
    : Entity(maxHp)
{
}

void Player::Update() {
    Def_ = 0;

    Effects_[static_cast<size_t>(Effect::Strength)] += Effects_[static_cast<size_t>(Effect::Ritual)];

    for (size_t i = 0; i < static_cast<size_t>(Effect::N_TEMPORARY_EFFECTS); i++) {
        if (Effects_[i]) {
            Effects_[i]--;
        }
    }
}

void Player::Reset() {
    Hp_ = MaxHp_;
    Def_ = 0;
}

Json Player::ToJson() const {
    Json res;
    res["max_hp"] = MaxHp_;
    res["hp"] = Hp_;
    res["def"] = Def_;
    res["effects"] = Json::array();
    for (size_t i = 0; i < static_cast<size_t>(Effect::N_EFFECTS); i++) {
        res["effects"].emplace_back(Effects_[i]);
    }
    return res;
}

Player Player::FromJson(const Json& info) {
    Player player(info["max_hp"]);
    player.Hp_ = info["hp"];
    player.Def_ = info["def"];
    for (const auto& e : info["effects"]) {
        player.Effects_[e[0]] = e[1];
    }
    return player;
}
