#include "Action.h"
#include <sstream>
#include <string>

Action::Action(Position from, Position to, Turn turn) {
    std::stringstream ss;
    ss << "Turn: ";
    if (turn == Turn::White) {
        ss << "white";
    } else {
        ss << "black";
    }
    ss << " Pawn from ";
    ss << std::to_string(from.Row) << std::to_string(from.Column);
    ss << " to ";
    ss << std::to_string(to.Row) << std::to_string(to.Column);
}
