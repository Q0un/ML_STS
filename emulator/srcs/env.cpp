#include "env.h"

Env::Env()
    : Player_(80)
{
    Logs_ = std::ofstream(std::string(PROJECT_DIR) + "/env.log");

    MaxEnergy_ = 3;

    LoadCards();
    for (size_t i = 0; i < 5; i++) {
        Deck_.emplace_back(0);
    }
    for (size_t i = 0; i < 4; i++) {
        Deck_.emplace_back(1);
    }
    Deck_.emplace_back(2);
    GameState_ = State::Nothing;
}

Env::~Env() {
    Logs_.close();
}

void Env::Reset() {
#ifdef LOGGING
    Logs_ << "===============RESET===============\n";
#endif
    GameState_ = State::Nothing;
    Player_.Reset();
    Pool_.clear();
    Hand_.clear();
    OffPool_.clear();
    Mobs_.clear();

    Deck_.clear();
    for (int i = 0; i < 5; i++) {
        Deck_.emplace_back(0);
    }
    for (int i = 0; i < 4; i++) {
        Deck_.emplace_back(1);
    }
    Deck_.emplace_back(2);

#ifdef LOGGING
    Logs_ << GetState() << std::endl;
#endif
    UpdateActions();
}

void Env::LoadCards() {
    CardPool_.emplace_back(CardType::Attack, 6, 0, 1);
    CardPool_.emplace_back(CardType::Skill, 0, 5, 1);
    Card card(CardType::Attack, 8, 0, 2);
    card.AddEffect(CardEffect::Vulnerable, 2);
    CardPool_.emplace_back(card);
}

void Env::StartFight(uint32_t mobSetId) {
    GameState_ = State::Fight;

    if (mobSetId == 0) {
        Mobs_.emplace_back(new JawWorm());
    } else if (mobSetId == 1) {
        Mobs_.emplace_back(new Cultist());
    }

    Energy_ = MaxEnergy_;
    Pool_.clear();
    Hand_.clear();
    OffPool_.clear();
    for (const auto& card : Deck_) {
        Pool_.emplace_back(card);
    }
    shuffle(Pool_.begin(), Pool_.end(), Random::GetInstance().GetRndGenerator());
    for (size_t i = 0; i < IN_HAND; i++) {
        Hand_.emplace_back(Pool_.back());
        Pool_.pop_back();
    }

    UpdateActions();
}

json Env::GetState() {
    json state;
    state["game_state"] = GameState_;
    if (GameState_ == State::Nothing) {
        return state;
    } else {
        state["max_energy"] = MaxEnergy_;
        state["player"] = Player_.ToJson();
        state["deck"] = Deck_;
        if (GameState_ == State::Fight) {
            state["energy"] = Energy_;
            state["mobs"] = json::array();
            for (const auto& mob : Mobs_) {
                state["mobs"].emplace_back(mob->ToJson());
            }

            state["hand"] = Hand_;
            state["pool"] = Pool_;
            state["offpool"] = OffPool_;
        }
    }
    return state;
}

void Env::UpdateHand() {
    Player_.Update();
    Energy_ = MaxEnergy_;
    while (!Hand_.empty()) {
        OffPool_.emplace_back(Hand_.back());
        Hand_.pop_back();
    }
    for (size_t i = 0; i < IN_HAND; i++) {
        if (Pool_.empty()) {
            while (!OffPool_.empty()) {
                Pool_.emplace_back(OffPool_.back());
                OffPool_.pop_back();
            }
            shuffle(Pool_.begin(), Pool_.end(), Random::GetInstance().GetRndGenerator());
        }
        Hand_.emplace_back(Pool_.back());
        Pool_.pop_back();
    }
}

void Env::MobTurn() {
    for (const auto& mob : Mobs_) {
        mob->Move(Player_);
    }
}

void Env::UseCard(uint32_t card, int32_t mob) {
    if (Energy_ < CardPool_[Hand_[card]].GetCost()) {
        return;
    }
    Energy_ -= CardPool_[Hand_[card]].GetCost();
    if (mob != -1) {
        CardPool_[Hand_[card]].Use(&Player_, Mobs_[mob]);
    } else {
        CardPool_[Hand_[card]].Use(&Player_);
    }
    OffPool_.emplace_back(Hand_[card]);
    Hand_.erase(Hand_.begin() + card);
}

double Env::Step(const Action& act) {
#ifdef LOGGING
    Logs_ << act << std::endl;
#endif
    double rew = 0;
    if (GameState_ == State::Nothing) {
        if (act.GetType() == ActionType::Play) {
            StartFight(act.GetArgs()[0]);
        } else {
            assert(0);
        }
    } else if (GameState_ == State::Fight) {
        if (act.GetType() == ActionType::Play) {
            uint32_t card = act.GetArgs()[0];
            int32_t mob = -1;
            uint32_t mobsHpBefore = MobsHp();
            if (act.GetArgs().size() > 1) {
                mob = act.GetArgs()[1];
            }
            UseCard(card, mob);
            rew = mobsHpBefore - MobsHp();
        } else if (act.GetType() == ActionType::End) {
            uint32_t playerHpBefore = Player_.GetHp();
            MobTurn();
            rew = -2 * double(playerHpBefore - Player_.GetHp());


            if (Player_.Dead()) {
                GameState_ = State::Lose;
            } else {
                UpdateHand();
            }
        } else {
            assert(0);
        }
        for (size_t i = 0; i < Mobs_.size(); i++) {
            if (Mobs_[i]->Dead()) {
                Mobs_.erase(Mobs_.begin() + i);
                i--;
            }
        }
        if (Mobs_.empty()) {
            GameState_ = State::Win;
        }
    } else {
        assert(0);
    }

    json res = GetState();
#ifdef LOGGING
    Logs_ << res << std::endl;
#endif

    UpdateActions();

    return rew;
}

State Env::GetGamestate() const {
    return GameState_;
}

std::vector<Action> Env::GetActs() const {
    return AvailableActs_;
}

Action Env::SampleAct() const {
    size_t ind = Random::GetInstance()(AvailableActs_.size());
    return AvailableActs_[ind];
}

uint32_t Env::MobsHp() const {
    uint32_t res = 0;
    for (const auto& mob : Mobs_) {
        res += mob->GetHp();
    }
    return res;
}

void Env::UpdateActions() {
    AvailableActs_.clear();
    if (GameState_ == State::Nothing) {
        AvailableActs_.emplace_back(ActionType::Play, std::vector<int32_t>({0}));
        AvailableActs_.emplace_back(ActionType::Play, std::vector<int32_t>({1}));
    } else if (GameState_ == State::Fight) {
        AvailableActs_.emplace_back(ActionType::End);
        for (size_t i = 0; i < Hand_.size(); i++) {
            if (CardPool_[Hand_[i]].GetCost() <= Energy_) {
                if (CardPool_[Hand_[i]].GetType() == CardType::Attack) {
                    for (size_t j = 0; j < Mobs_.size(); j++) {
                        AvailableActs_.emplace_back(ActionType::Play, std::vector<int32_t>({i, j}));
                    }
                } else if (CardPool_[Hand_[i]].GetType() == CardType::Skill) {
                    AvailableActs_.emplace_back(ActionType::Play, std::vector<int32_t>({i}));
                }
            }
        }
    }
}

void Env::PrintState() {
    std::cout << GetState() << std::endl;
    json acts = json::array();
    for (const auto& i : GetActs()) {
        acts.emplace_back(i.ToJson());
    }
    std::cout << acts << std::endl;
}
