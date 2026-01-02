#pragma once

#include <fstream>
#include <iostream>

#include "action.h"
#include "cards.h"
#include "mob.h"
#include "player.h"
#include "random.h"

#include "nlohmann/json.hpp"

using json = nlohmann::json;

enum class State{
    Fight,
    Lose,
    Win,
    Nothing,
    N_STATES
};

class Env {
public:
    Env();
    ~Env();

    void Reset();
    double Step(const Action& act);
    std::vector<Action> GetActs() const;
    void PrintState();

private:
    void LoadCards();
    void StartFight(uint32_t mobSetId);
    json GetState();
    void UpdateHand();
    void MobTurn();
    void UseCard(uint32_t card, int32_t mob);
    State GetGamestate() const;
    Action SampleAct() const;
    uint32_t MobsHp() const;
    void UpdateActions();

private:
    std::vector<Card> CardPool_;
    std::vector<uint32_t> Deck_;
    std::vector<uint32_t> Hand_;
    std::vector<uint32_t> Pool_;
    std::vector<uint32_t> OffPool_;
    std::vector<Mob*> Mobs_;
    Player Player_;
    State GameState_;
    std::ofstream Logs_;
    uint32_t MaxEnergy_;
    uint32_t Energy_;
    std::vector<Action> AvailableActs_;

    static constexpr size_t IN_HAND = 5;
};
