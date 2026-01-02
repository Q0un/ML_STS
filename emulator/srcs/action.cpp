#include "action.h"

namespace {

const std::vector<std::string> ACTION_TYPE_STR = {
    "play",
    "end"
};

} // anonymous namespace

Action::Action(ActionType type, const std::vector<int32_t>& args)
    : Type_(type)
    , Args_(args)
{
}

ActionType Action::GetType() const {
    return Type_;
}

const std::vector<int32_t>& Action::GetArgs() const {
    return Args_;
}

Json Action::ToJson() const {
    Json res;
    res["type"] = Type_;
    res["args"] = Args_;
    return res;
}

std::ofstream& operator<<(std::ofstream& out, const Action& action) {
    out << ACTION_TYPE_STR[static_cast<size_t>(action.Type_)] << ' ';
    for (const auto& arg : action.Args_) {
        out << arg << ' ';
    }
    return out;
}
