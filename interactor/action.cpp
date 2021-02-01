#include "action.h"
using namespace std;

vector<string> actionS = {"play", "end"};

Action::Action(act_type type, const vector<int> &args) : type(type), args(args) {}

ofstream& operator<<(ofstream &out, const Action &act) {
    out << actionS[act.type] << ' ';
    for (auto &i : act.args) {
        out << i << ' ';
    }
    return out;
}
