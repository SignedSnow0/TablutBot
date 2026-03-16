#include "TablutWriter.h"

#include <nlohmann/json.hpp>
#include <string>

#include "state/Tablut.h"

using json = nlohmann::json;

std::string positionToString(const Position &position) {
    char column = (char)(position.Column + 97);
    std::string string = "";
    string.append(&column);
    string.append(std::to_string(position.Row + 1));

    return string;
}

TablutSocketWriter::TablutSocketWriter(const std::shared_ptr<Socket> &socket)
    : mSocket(socket) {}

void TablutSocketWriter::WriteMove(const Position &fromPosition,
                                   const Position &toPosition,
                                   bool whiteTurn) const {
    auto outMsg = json::object();
    outMsg["from"] = positionToString(fromPosition);
    outMsg["to"] = positionToString(toPosition);
    outMsg["turn"] = whiteTurn ? "WHITE" : "BLACK";

    mSocket->Send(outMsg.dump());
}
