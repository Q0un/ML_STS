#pragma once

#include <fstream>
#include <vector>

#include "json.h"

enum class ActionType {
    Play,
    End
};

class Action {
public:
    Action(ActionType type, const std::vector<int32_t>& args = {});

    ActionType GetType() const;
    const std::vector<int32_t>& GetArgs() const;

    Json ToJson() const;
    friend std::ofstream& operator<<(std::ofstream& out, const Action& action);

private:
    ActionType Type_;
    std::vector<int32_t> Args_;
};
