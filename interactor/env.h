#ifndef STS_PROJECT_ENV_H
#define STS_PROJECT_ENV_H

#include "cards.h"
#include "player.h"
#include "random.h"
#include "mob.h"
#include "action.h"
#include "nlohmann/json.hpp"
#include <random>
#include <fstream>
#include <iostream>
using json = nlohmann::json;

enum class State{Fight, Lose, Win, Nothing};

class Env {
    std::vector<Card> card_pool;
    std::vector<int> deck;
    std::vector<int> hand, pool, offpool;
    std::vector<Mob*> mobs;
    Player player;
    State game_state;
    const int IN_HAND = 5;
    std::ofstream logs;
    int max_energy, energy;
    std::vector<Action> available_acts;

public:
    Env();
    ~Env();

    void loadCards();
    void reset();
    void startFight(int mob_set);
    json getState();
    void updateHand();
    void mobTurn();
    void useCard(int card, int mob);
    double step(const Action &act);
    State getGamestate() const;
    std::vector<Action> getActs() const;
    Action sampleAct() const;
    int mobsHp() const;
    void updateActions();
    void printState();
    int sumDmg() const;
};

#endif //STS_PROJECT_ENV_H
