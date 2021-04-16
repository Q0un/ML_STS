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

    json get_json() const;
    void update();
    void load(json &info);
    void reset();
    int get_dhp() const;
};

#endif //STS_PROJECT_PLAYER_H
