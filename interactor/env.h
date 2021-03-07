#pragma once
#include "cards.h"
#include "player.h"
#include "mob.h"
#include "action.h"
#include "nlohmann/json.hpp"
#include <random>
#include <fstream>
using json = nlohmann::json;

enum State{fight, lose, win, nothing};

class Env {
    std::vector<Card> deck;
    std::vector<int> hand, pool, offpool;
    vector<Mob> mobs;
    Player player;
    State game_state;
    const int IN_HAND = 5;
    ofstream logs;
    int max_energy, energy;
    vector<Action> available_acts;
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
    void load(json &info);
    std::pair<json, double> get_result(json &snapshot, const Action &act);
    std::vector<Action> get_acts() const;
    Action sample_act() const;
    int mobs_hp() const;
    void update_actions();
};