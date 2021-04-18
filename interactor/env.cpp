#include "env.h"

Env::Env() {
    logs = std::ofstream(std::string(PROJECT_DIR) + "/env.log");

    max_energy = 3;
    player = Player(80);
    for (int i = 0; i < 5; i++) {
        deck.emplace_back(Card_type::attack, 6, 0, 1);
    }
    for (int i = 0; i < 4; i++) {
        deck.emplace_back(Card_type::skill, 0, 5, 1);
    }
    auto card = Card(Card_type::attack, 8, 0, 2);
    card.addEffect(Card_effect::vulnerable, 2);
    deck.emplace_back(card);
    game_state = State::nothing;

    logs << getState() << std::endl;
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
    card.addEffect(Card_effect::vulnerable, 2);
    deck.emplace_back(card);
}

void Env::startFight() {
    game_state = State::fight;

    mobs.emplace_back(new JawWorm());
    mobhp_boof = mobsHp();

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

    updateActions();
}

json Env::getState() {
    json st;
    st["game_state"] = game_state;
    if (game_state == State::fight) {
        st["max_energy"] = max_energy;
        st["energy"] = energy;
        st["player"] = player.getJson();
        st["mobs"] = json::array();
        for (auto &mob : mobs) {
            st["mobs"].emplace_back(mob->getJson());
        }

        st["deck"] = json::array();
        for (auto &card : deck) {
            st["deck"].emplace_back(card.getJson());
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

void Env::updateHand() {
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

void Env::mobTurn() {
    for (auto &mob : mobs) {
        mob->move(player);
    }
}

void Env::useCard(int card, int mob) {
    if (energy < deck[hand[card]].getCost()) {
        return;
    }
    energy -= deck[hand[card]].getCost();
    if (mob != -1) {
        deck[hand[card]].use(&player, mobs[mob]);
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
        useCard(card, mob);
    } else if (act.type == act_type::end) {
        int playerhp_boof = player.getHp();
        mobTurn();

        rew = (mobhp_boof - mobsHp()) * 2 + (playerhp_boof - player.getHp());

        if (player.dead()) {
            game_state = State::lose;
        } else {
            updateHand();
        }
    }
    for (int i = 0; i < mobs.size(); i++) {
        if (mobs[i]->dead()) {
            mobs.erase(mobs.begin() + i);
            i--;
        }
    }
    if (mobs.empty()) {
        game_state = State::win;
    }
    json res = getState();
    logs << res << std::endl;

    updateActions();

    return {res, rew};
}

State Env::getGamestate() const {
    return game_state;
}

std::vector<Action> Env::getActs() const {
    return available_acts;
}

Action Env::sampleAct() const {
    int ind = rnd()%available_acts.size();
    return available_acts[ind];
}

int Env::mobsHp() const {
    int res = 0;
    for (auto &mob : mobs) {
        res += mob->getHp();
    }
    return res;
}

void Env::updateActions() {
    available_acts.clear();
    available_acts.emplace_back(act_type::end);
    for (int i = 0; i < hand.size(); i++) {
        if (deck[hand[i]].getCost() <= energy) {
            if (deck[hand[i]].getType() == Card_type::attack) {
                for (int j = 0; j < mobs.size(); j++) {
                    available_acts.emplace_back(act_type::play, std::vector<int>({i, j}));
                }
            } else if (deck[hand[i]].getType() == Card_type::skill) {
                available_acts.emplace_back(act_type::play, std::vector<int>({i}));
            }
        }
    }
}
