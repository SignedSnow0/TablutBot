#pragma once
#include "state/Tablut.h"

enum class Turn { White, Black };

class Action {
public:
    Action(Position from, Position to, Turn turn);

private:
    std::string mMessage;
};
