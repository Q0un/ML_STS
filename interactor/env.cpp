#include "env.h"

Env::Env() {
    logs = std::ofstream(std::string(PROJECT_DIR) + "/env.log");

    max_energy = 3;
    player = Player(80);

    loadCards();
    for (int i = 0; i < 5; i++) {
        deck.emplace_back(0);
    }
    for (int i = 0; i < 4; i++) {
        deck.emplace_back(1);
    }
    deck.emplace_back(2);
    game_state = State::Nothing;
}

Env::~Env() {
    logs.close();
}

void Env::reset() {
#ifdef LOGGING
    logs << "===============RESET===============\n";
#endif
    game_state = State::Nothing;
    player.reset();
    pool.clear();
    hand.clear();
    offpool.clear();
    mobs.clear();

    deck.clear();
    for (int i = 0; i < 5; i++) {
        deck.emplace_back(0);
    }
    for (int i = 0; i < 4; i++) {
        deck.emplace_back(1);
    }
    deck.emplace_back(2);

#ifdef LOGGING
    logs << getState() << std::endl;
#endif
    updateActions();
}

void Env::loadCards() {
    card_pool.emplace_back(CardType::Attack, 6, 0, 1);
    card_pool.emplace_back(CardType::Skill, 0, 5, 1);
    auto card = Card(CardType::Attack, 8, 0, 2);
    card.addEffect(CardEffect::Vulnerable, 2);
    card_pool.emplace_back(card);
}

void Env::startFight() {
    game_state = State::Fight;

    mobs.emplace_back(new JawWorm());

    energy = max_energy;
    pool.clear();
    hand.clear();
    offpool.clear();
    for (auto &i : deck) {
        pool.emplace_back(i);
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
    if (game_state == State::Nothing) {
        return st;
    } else {
        st["max_energy"] = max_energy;
        st["player"] = player.getJson();
        st["deck"] = deck;
        if (game_state == State::Fight) {
            st["energy"] = energy;
            st["mobs"] = json::array();
            for (auto &mob : mobs) {
                st["mobs"].emplace_back(mob->getJson());
            }

            st["hand"] = hand;
            st["pool"] = pool;
            st["offpool"] = offpool;
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
    if (energy < card_pool[hand[card]].getCost()) {
        return;
    }
    energy -= card_pool[hand[card]].getCost();
    if (mob != -1) {
        card_pool[hand[card]].use(&player, mobs[mob]);
    } else {
        card_pool[hand[card]].use(&player);
    }
    offpool.emplace_back(hand[card]);
    hand.erase(hand.begin() + card);
}

double Env::step(const Action &act) {
#ifdef LOGGING
    logs << act << std::endl;
#endif
    double rew = 0;
    if (game_state == State::Nothing) {
        if (act.type == ActType::Play) {
            startFight();
        } else {
            assert(0);
        }
    } else if (game_state == State::Fight) {
        if (act.type == ActType::Play) {
            int card = act.args[0];
            int mob = -1;
            int mobhp_boof = mobsHp();
            int dmg_remained = std::max(sumDmg() - player.getDef(), 0);
            int def_boof = player.getDef();
            if (act.args.size() > 1) {
                mob = act.args[1];
            }
            useCard(card, mob);
            rew = (mobhp_boof - mobsHp()) + 2 * std::min(dmg_remained, player.getDef() - def_boof);
            if (dmg_remained == 0 && player.getDef() - def_boof > 0) {
                rew -= 100;
            }
        } else if (act.type == ActType::End) {
            int playerhp_boof = player.getHp();
            rew = -2 * sumDmg();
            mobTurn();


            if (player.dead()) {
                game_state = State::Lose;
            } else {
                updateHand();
            }
        } else {
            assert(0);
        }
        for (int i = 0; i < mobs.size(); i++) {
            if (mobs[i]->dead()) {
                mobs.erase(mobs.begin() + i);
                i--;
            }
        }
        if (mobs.empty()) {
            game_state = State::Win;
        }
    } else {
        assert(0);
    }

    json res = getState();
#ifdef LOGGING
    logs << res << std::endl;
#endif

    updateActions();

    return rew;
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

int Env::sumDmg() const {
    int dmg = 0;
    for (auto mob : mobs) {
        dmg += mob->getDmg();
    }
    return dmg;
}

void Env::updateActions() {
    available_acts.clear();
    if (game_state == State::Nothing) {
        available_acts.emplace_back(ActType::Play);
    } else if (game_state == State::Fight) {
        available_acts.emplace_back(ActType::End);
        for (int i = 0; i < hand.size(); i++) {
            if (card_pool[hand[i]].getCost() <= energy) {
                if (card_pool[hand[i]].getType() == CardType::Attack) {
                    for (int j = 0; j < mobs.size(); j++) {
                        available_acts.emplace_back(ActType::Play, std::vector<int>({i, j}));
                    }
                } else if (card_pool[hand[i]].getType() == CardType::Skill) {
                    available_acts.emplace_back(ActType::Play, std::vector<int>({i}));
                }
            }
        }
    } else {

    }
}

void Env::printState() {
    std::cout << getState() << std::endl;
    json acts = json::array();
    for (auto &i : getActs()) {
        acts.emplace_back(i.getJson());
    }
    std::cout << acts << std::endl;
}
