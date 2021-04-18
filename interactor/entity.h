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

    int dealDmg(int dmg) const;
    void takeDmg(int dmg);
    void addEffect(Effect e, int val);
    void addDef(int val);
    bool dead() const;
    int getHp() const;
};

#endif //STS_PROJECT_ENTITY_H