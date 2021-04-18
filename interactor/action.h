#ifndef STS_PROJECT_ACTION_H
#define STS_PROJECT_ACTION_H

#include <vector>
#include <fstream>
#include "nlohmann/json.hpp"
using json = nlohmann::json;

enum class actType{Play, End};

class Action {
public:
    actType type;
    std::vector<int> args;

    Action() = default;
    Action(actType type, const std::vector<int> &args = {});

    json getJson() const;
    friend std::ofstream& operator<<(std::ofstream &out, const Action &act);
};

#endif //STS_PROJECT_ACTION_H