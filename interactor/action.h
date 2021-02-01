#pragma once
#include <vector>
#include <fstream>
using namespace std;

enum act_type{play, end};

class Action {
public:
    act_type type;
    vector<int> args;

    Action() = default;
    Action(act_type type, const vector<int> &args = {});

    friend ofstream& operator<<(ofstream &out, const Action &act);
};