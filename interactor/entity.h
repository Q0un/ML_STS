#ifndef STS_PROJECT_ENTITY_H
#define STS_PROJECT_ENTITY_H

#include <vector>

enum class Effect{vulnerable, N_TEMPS, strength, N_EFFECTS};

class Entity {
protected:
    int max_hp, hp, def;
    int effects[(int)Effect::N_EFFECTS];

public:
    Entity() = default;
    Entity(int max_hp);

    int deal_dmg(int dmg) const;
    void take_dmg(int dmg);
    void add_effect(Effect e, int val);
    void add_def(int val);
    bool dead() const;
    int get_hp() const;
};

#endif //STS_PROJECT_ENTITY_H