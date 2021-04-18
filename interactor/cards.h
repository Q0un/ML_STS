#ifndef STS_PROJECT_CARDS_H
#define STS_PROJECT_CARDS_H

#include <vector>
#include "entity.h"
#include "nlohmann/json.hpp"
using json = nlohmann::json;

enum class Card_effect{vulnerable, N_CEFFECTS};
enum class Card_type{attack, skill};

class Card {
    Card_type type;
    int dmg, def, cost;
    std::vector<std::pair<Card_effect, int>> effects;

public:
    Card() = default;
    Card(Card_type type, int dmg, int def, int cost);
    Card(json &info);

    void addEffect(Card_effect e, int val);
    json getJson() const;
    int getCost() const;
    void use(Entity *player, Entity *mob = nullptr) const;
    void load(json &info);
    Card_type getType() const;
};

#endif //STS_PROJECT_CARDS_H
