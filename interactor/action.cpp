#include "action.h"

std::vector<std::string> actionS = {"play", "end"};

Action::Action(act_type type, const std::vector<int> &args) : type(type), args(args) {}

std::ofstream& operator<<(std::ofstream &out, const Action &act) {
    out << actionS[(int)act.type] << ' ';
    for (auto &i : act.args) {
        out << i << ' ';
    }
    return out;
}
