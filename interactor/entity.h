#pragma once
#include <vector>

enum Effect{vulnerable, N_TEMPS, strength, N_EFFECTS};

class Entity {
protected:
    int max_hp, hp, def;
    int effects[N_EFFECTS];

public:
    Entity() = default;
    Entity(int max_hp);

    void give_dmg(int dmg);
    void add_effect(Effect e, int val);
    void add_def(int val);
    bool dead() const;
    int get_hp() const;
};