#pragma once

#include "entity.h"
#include "cards.h"
#include "json.h"

class Player : public Entity {
public:
    Player(uint32_t maxHp);

    void Update();
    void Reset();

    Json ToJson() const;
    static Player FromJson(const Json& info);
};
