#include "action.h"

std::vector<std::string> actionS = {"play", "end"};

Action::Action(actType type, const std::vector<int> &args) : type(type), args(args) {}

json Action::getJson() const {
    json res;
    res["type"] = type;
    res["args"] = args;
    return res;
}

std::ofstream& operator<<(std::ofstream &out, const Action &act) {
    out << actionS[(int)act.type] << ' ';
    for (auto &i : act.args) {
        out << i << ' ';
    }
    return out;
}
