#ifndef STS_PROJECT_PLAYER_H
#define STS_PROJECT_PLAYER_H

#include "entity.h"
#include "cards.h"
#include "nlohmann/json.hpp"
using json = nlohmann::json;

class Player : public Entity {

public:
    Player() = default;
    Player(int max_hp);
    Player(json &info);

    json getJson() const;
    void update();
    void load(json &info);
    void reset();
};

#endif //STS_PROJECT_PLAYER_H
