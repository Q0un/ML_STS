#ifndef STS_PROJECT_ACTION_H
#define STS_PROJECT_ACTION_H

#include <vector>
#include <fstream>

enum class act_type{play, end};

class Action {
public:
    act_type type;
    std::vector<int> args;

    Action() = default;
    Action(act_type type, const std::vector<int> &args = {});

    friend std::ofstream& operator<<(std::ofstream &out, const Action &act);
};

#endif //STS_PROJECT_ACTION_H