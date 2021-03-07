#include "env.h"
#include "random.h"

Env::Env() {
    logs = std::ofstream("../env.log");

    max_energy = 3;
    player = Player(80);
    for (int i = 0; i < 5; i++) {
        deck.emplace_back(Card_type::attack, 6, 0, 1);
    }
    for (int i = 0; i < 4; i++) {
        deck.emplace_back(Card_type::skill, 0, 5, 1);
    }
    auto card = Card(Card_type::attack, 8, 0, 2);
    card.add_effect(Card_effect::Cvulnerable, 2);
    deck.emplace_back(card);
    game_state = State::nothing;

    logs << get_state() << std::endl;
}

Env::~Env() {
    logs.close();
}

void Env::reset() {
    logs << "===============RESET===============\n";
    game_state = State::nothing;
    player.reset();
    pool.clear();
    hand.clear();
    offpool.clear();
    mobs.clear();

    deck.clear();
    for (int i = 0; i < 5; i++) {
        deck.emplace_back(Card_type::attack, 6, 0, 1);
    }
    for (int i = 0; i < 4; i++) {
        deck.emplace_back(Card_type::skill, 0, 5, 1);
    }
    auto card = Card(Card_type::attack, 8, 0, 2);
    card.add_effect(Card_effect::Cvulnerable, 2);
    deck.emplace_back(card);
}

void Env::start_fight() {
    game_state = State::fight;

    mobs.emplace_back(38 + rnd()%5, 10);
    mobhp_boof = mobs_hp();

    energy = max_energy;
    pool.clear();
    hand.clear();
    offpool.clear();
    for (int i = 0; i < deck.size(); i++) {
        pool.push_back(i);
    }
    shuffle(pool.begin(), pool.end(), rnd);
    for (int i = 0; i < IN_HAND; i++) {
        hand.emplace_back(pool.back());
        pool.pop_back();
    }

    update_actions();
}

json Env::get_state() {
    json st;
    st["game_state"] = game_state;
    if (game_state == State::fight) {
        st["energy"] = energy;
        // st["max_energy"] = max_energy;
        st["player"] = player.get_json();
        st["mobs"] = json::array();
        for (auto &mob : mobs) {
            st["mobs"].emplace_back(mob.get_json());
        }

        st["deck"] = json::array();
        for (auto &card : deck) {
            st["deck"].emplace_back(card.get_json());
        }
        st["hand"] = json::array();
        for (auto &card : hand) {
            st["hand"].emplace_back(card);
        }
        st["pool"] = json::array();
        for (auto &card : pool) {
            st["pool"].emplace_back(card);
        }
        st["offpool"] = json::array();
        for (auto &card : offpool) {
            st["offpool"].emplace_back(card);
        }
    }
    return st;
}

void Env::update_hand() {
    player.update();
    energy = max_energy;
    while (!hand.empty()) {
        offpool.emplace_back(hand.back());
        hand.pop_back();
    }
    for (int i = 0; i < IN_HAND; i++) {
        if (pool.empty()) {
            while (!offpool.empty()) {
                pool.emplace_back(offpool.back());
                offpool.pop_back();
            }
            shuffle(pool.begin(), pool.end(), rnd);
        }
        hand.emplace_back(pool.back());
        pool.pop_back();
    }
}

void Env::mob_turn() {
    for (auto &mob : mobs) {
        mob.move(player);
    }
}

void Env::use_card(int card, int mob) {
    if (energy < deck[hand[card]].get_cost()) {
        return;
    }
    energy -= deck[hand[card]].get_cost();
    if (mob != -1) {
        deck[hand[card]].use(&player, &mobs[mob]);
    } else {
        deck[hand[card]].use(&player);
    }
    offpool.emplace_back(hand[card]);
    hand.erase(hand.begin() + card);
}

std::pair<json, double> Env::step(const Action &act) {
    logs << act << ' ';
    logs << std::endl;
    double rew = 0;
    if (act.type == act_type::play) {
        int card = act.args[0];
        int mob = -1;
        if (act.args.size() > 1) {
            mob = act.args[1];
        }
        use_card(card, mob);
    } else if (act.type == act_type::end) {
        int playerhp_boof = player.get_hp();
        mob_turn();

        rew = (mobhp_boof - mobs_hp()) * 2 + (playerhp_boof - player.get_hp());

        if (player.dead()) {
            game_state = State::lose;
        } else {
            update_hand();
        }
    }
    for (int i = 0; i < mobs.size(); i++) {
        if (mobs[i].dead()) {
            mobs.erase(mobs.begin() + i);
            i--;
        }
    }
    if (mobs.empty()) {
        game_state = State::win;
    }
    json res = get_state();
    logs << res << std::endl;

    update_actions();

    return {res, rew};
}

State Env::get_gamestate() const {
    return game_state;
}

void Env::load(json &info) {
    game_state = info["game_state"];
    if (game_state == State::fight) {
        player.load(info["player"]);
        mobs.clear();
        for (auto &mob : info["mobs"]) {
            mobs.emplace_back(mob);
        }
        energy = info["energy"];
        hand.clear();
        offpool.clear();
        pool.clear();
        deck.clear();
        for (auto &card : info["deck"]) {
            deck.emplace_back(card);
        }
        for (auto &card : info["hand"]) {
            hand.emplace_back(card);
        }
        for (auto &card : info["pool"]) {
            pool.emplace_back(card);
        }
        for (auto &card : info["offpool"]) {
            offpool.emplace_back(card);
        }
    }
    update_actions();
}

std::pair<json, double> Env::get_result(json &snapshot, const Action &act) {
    auto snap2 = get_state();
    load(snapshot);
    auto res = step(act);
    load(snap2);
    return res;
}

std::vector<Action> Env::get_acts() const {
    return available_acts;
}

Action Env::sample_act() const {
    int ind = rnd()%available_acts.size();
    return available_acts[ind];
}

int Env::mobs_hp() const {
    int res = 0;
    for (auto &mob : mobs) {
        res += mob.get_hp();
    }
    return res;
}

void Env::update_actions() {
    available_acts.clear();
    available_acts.emplace_back(act_type::end);
    for (int i = 0; i < hand.size(); i++) {
        if (deck[hand[i]].get_cost() <= energy) {
            if (deck[hand[i]].get_type() == Card_type::attack) {
                for (int j = 0; j < mobs.size(); j++) {
                    available_acts.emplace_back(act_type::play, std::vector<int>({i, j}));
                }
            } else if (deck[hand[i]].get_type() == Card_type::skill) {
                available_acts.emplace_back(act_type::play, std::vector<int>({i}));
            }
        }
    }
}
