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
using json = nlohmann::json;

enum class State{fight, lose, win, nothing};

class Env {
    std::vector<Card> deck;
    std::vector<int> hand, pool, offpool;
    std::vector<Mob*> mobs;
    Player player;
    State game_state;
    const int IN_HAND = 5;
    std::ofstream logs;
    int max_energy, energy;
    std::vector<Action> available_acts;
    int mobhp_boof;

public:
    Env();
    ~Env();

    void reset();
    void start_fight();
    json get_state();
    void update_hand();
    void mob_turn();
    void use_card(int card, int mob);
    std::pair<json, double> step(const Action &act);
    State get_gamestate() const;
    std::vector<Action> get_acts() const;
    Action sample_act() const;
    int mobs_hp() const;
    void update_actions();
};

#endif //STS_PROJECT_ENV_H
